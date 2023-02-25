# Written by Gihwan Kim, CNU

import cv2
import pandas as pd
import os

import sys
# https://devpouch.tistory.com/99

# [shell format]
# python *.py {dataset-dir} {reiszed-dir} {data-height} {data-width}

# for arguemtns.
FLAGS = ''
FILE_SET_NUM = 400



'''
[!!!Common logic rule!!!]
    Before iterate every directory using 'for something in {os.listdir()}`
    We have to sort result of `os.listdir` to  set same file order.
'''


'''
[ConvertToSpecificSize()]
/ Descrpiton /
- If you want preserve image quality set option to cv2.IMREAD_UNCHANGED
  But, we highly recommend to use gray scale
- If you want clear image you can set low sigma value
- If sigma set higher value iamge will be blured.


/ Parameters /
- target_dim: tuple, shape of resized image
- src_path: string, source file path


/ Retrun /
- img_smoothed: resized image, output image of the same size and type as src.
'''
def ConvertToSpecificSize(target_dim, src_path):
    # - if you want preserve image quality set option to cv2.IMREAD_UNCHANGED
    #   but we highly recommend to use gray scale
    # src_image = cv2.imread(src_path, cv2.IMREAD_UNCHANGED)
    src_image = cv2.imread(src_path, cv2.IMREAD_GRAYSCALE)

    # resize image
    resized = cv2.resize(src_image, target_dim, interpolation=cv2.INTER_AREA)

    # If you want clear image you can set low sigma value
    # If sigma set higher value iamge will be blured.
    sigma = 0.6
    img_smoothed = cv2.GaussianBlur(resized, (0,0), sigma)
    return (img_smoothed)


'''
[XmlToCSV()]

/ Descrpiton /
- You can split dataset each FILE_SET_NUM.
- convert annotated file (xml) to CSV file foramt
    convert shape and bounding box coordinates to target dimension scale

/ Parameters /
- src_shape: (height, width)
- target_dim: (FLAGS.dataheight, FLAGS.datawidth)
- src_dir: xml directory
- dst_dir: csv directory

/ Retrun /
- ret_xml_df: total xml file dataframe
- ret_xml_df_list: sperated dataframe for each FILE_SET_NUM
    [dataframe01, dataframe02, ...]
'''
def XmlToCSV(target_dim, src_dir, dst_dir, src_dataset_dir):
    import xml.etree.ElementTree as ET

    # if CSV directory is not exist create new directory.
    if not(os.path.isdir(dst_dir)):
        os.makedirs(dst_dir)

    # [for dataframe parameters]
    #   xml_elements_list: it used to store information of image data, it is row of datafrmae.
    xml_elements_list = [] # data
    # Column name list of data frame
    column_name = ['filename', 'width', 'height',
                    'class', 'xmin', 'ymin', 'xmax', 'ymax'] # classes

    # We split dataset each FILE_SET_NUM.
    file_id = 0 # for checking currnet file number
    directory_id = 0 # to split directory by number of FILE_SET_NUM

    # for return, dataframe of every xml data
    ret_xml_df = pd.DataFrame(columns=column_name)

    # It will stored [Train_dataframe, Valid_dataframe, Test_dataframe]
    ret_xml_df_list = []


    # iterate each annotatoin folders
    for folder_name in sorted(os.listdir(path=src_dir)):
        cur_folder_dir = os.path.join(src_dir, folder_name)

        # Prefix path for to store converted CSV
        prefix_resized_dir = os.path.join(dst_dir, folder_name)

        # Converted CSV file is sotred here.
        resized_dir = os.path.join(prefix_resized_dir, "annotation" + directory_id.__str__())

        cur_file_list_of_dataset_folder = sorted(os.listdir(path=os.path.join(src_dataset_dir, folder_name)))

        # idx: index of cur_folder_dir list
        for idx, xml_file_name in enumerate(sorted(os.listdir(path=cur_folder_dir))):

            # Read xml file and get data
            cur_file_dir = os.path.join(cur_folder_dir, xml_file_name)

            tree = ET.parse(cur_file_dir)

            # [bellow varialbles are information about xml tag]
            root_tag = tree.getroot()
            file_name = cur_file_list_of_dataset_folder[idx]
            size_tag = root_tag.find('size')
            object_tag = root_tag.find('object')

            # image shape
            src_width = int(size_tag.find('width').text)
            src_height = int(size_tag.find('height').text)

            # (height scale, width scale) = target_dim / src_shape
            scale_factor = (target_dim[0] / src_height, target_dim[1] / src_width)
            class_name = object_tag.find('name').text

            # coordinate (x, y)
            # order: 'xmin', 'ymin', 'xmax', 'ymax'
            bounding_box_tag = object_tag.find('bndbox')
            ##################################################

            # Element is row data of dataframe
            # Set xml tag informations to tuple type
            #   (filename, width, height, class, xmin, ymin, xmax, ymax)
            element = (file_name, target_dim[1], target_dim[0], class_name,

                        # convert bounding box coordinates to speicifc size
                        int(bounding_box_tag[0].text) * scale_factor[1],
                        int(bounding_box_tag[1].text) * scale_factor[0],
                        int(bounding_box_tag[2].text) * scale_factor[1],
                        int(bounding_box_tag[3].text) * scale_factor[0])

            xml_elements_list.append(element)

            # save dataframe to csv each FILE_SET_NUM
            if file_id % FILE_SET_NUM == 0:
                if not(os.path.isdir(resized_dir)):
                    os.makedirs(resized_dir)

                # Create xml data frame for each xml folders
                #   xml_folder0/*.xml --> xml_folder0.csv
                #   xml_folder1/*.xml --> xml_folder1.csv
                if file_id != 0:
                    xml_df = pd.DataFrame(data=xml_elements_list, columns=column_name)

                    # concate all dataframes for return dataframe
                    ret_xml_df = pd.concat([ret_xml_df, xml_df])

                    # dataframe to csv
                    xml_df.to_csv(os.path.join(resized_dir, directory_id.__str__() + '.csv'))

                    # clear buffer
                    xml_elements_list.clear()

                    # add current xml_df to list
                    ret_xml_df_list.append(xml_df)

                    # update resized_dir
                    directory_id += 1
                    resized_dir = os.path.join(prefix_resized_dir,
                                                "annotation" + directory_id.__str__())

            # update csv file id
            file_id += 1

        # If the number of data is not divisible by FILE_SET_NUM.
        # It has buffer dataframe. We have to clear buffer
        if len(xml_elements_list) > 0:
            if not(os.path.isdir(resized_dir)):
                os.makedirs(resized_dir)
            xml_df = pd.DataFrame(data=xml_elements_list, columns=column_name)
            # concate all dataframes for return dataframe
            ret_xml_df = pd.concat([ret_xml_df, xml_df])

            # create CSV
            xml_df.to_csv(os.path.join(resized_dir, directory_id.__str__() + '.csv'))

            # clear buffer
            xml_elements_list.clear()

            ret_xml_df_list.append(xml_df)

    print('--------------------------------------')
    print(f'check buffer={len(xml_elements_list)}')
    print(f'resized files={file_id}')
    print(f'xml df nums={len(ret_xml_df_list)}')

    # total.csv is saved to 'resizedannotationdir'
    ret_xml_df.to_csv(os.path.join(dst_dir, 'total.csv'))
    return (ret_xml_df, ret_xml_df_list)

'''
[Resizing()]
/ Descrpiton /
Image data is resized to specific shape (FLAGS.dataheight, FLAGS.datawidth)

/ Parameters /
- src_dir: string, source file path to resize image
- dst_dir: string, path of resized image


/ Retrun /
- no return
'''
def Resizing(src_dir, dst_dir):
    target_dim = (FLAGS.dataheight, FLAGS.datawidth)

    # We split dataset each FILE_SET_NUM.
    file_id = 0 # for checking currnet file number
    directory_id = 0 # to split directory by number of FILE_SET_NUM

    # iterate each annotation folders
    for folder_name in sorted(os.listdir(path=src_dir)):

        # Currnet annotation directory
        cur_folder_dir = path=os.path.join(src_dir, folder_name)

        # Resized annotation directory
        resized_dir = os.path.join(dst_dir, folder_name, "data" + directory_id.__str__())
        for file_name in sorted(os.listdir(cur_folder_dir)):
            cur_file_dir = os.path.join(cur_folder_dir, file_name)

            # checking if it is a file
            if os.path.isfile(cur_file_dir):
                _, file_expansion = os.path.splitext(file_name)

                # Check file is image.
                if file_expansion.lower().endswith(('jpeg', 'jpg', 'png')):
                    resized_image = ConvertToSpecificSize(target_dim, cur_file_dir)

                    # resized_directory: '{prefix_dir}/{folder_name}/data{id}'
                    # resized_dir = os.path.join(dst_dir, folder_name, "data" + directory_id.__str__())
                    tmp_resized_dir =resized_dir

                    # create new directory every FILE_SET_NUMth files
                    if file_id % FILE_SET_NUM == 0:
                        resized_dir = os.path.join(dst_dir, folder_name, "data" + directory_id.__str__())
                        directory_id += 1
                        if not(os.path.isdir(resized_dir)):
                            os.makedirs(resized_dir)\

                    # Create resized data
                    cv2.imwrite(os.path.join(tmp_resized_dir, file_name), resized_image)
                    file_id += 1



# This section is TFRcord section.

import io
import tensorflow as tf
from PIL import Image

from models.research.object_detection.utils import dataset_util, label_map_util
from collections import namedtuple

# return data of row_label in label_map_dict
def class_text_to_int(row_label, label_map_dict):
    return label_map_dict[row_label]


'''
[split()]

/ Descrpiton /
- split dataframe by each row.

/ Parameters /
- df: dataframe to split
- group: group name to group by

/ Retrun /
- grouped list
'''
def split(df, group):
    # divide dataframe of information to 'filename' and 'object'
    data = namedtuple('data', ['filename', 'object'])
    gb = df.groupby(group)

    # return divided rows to list
    return [data(filename, gb.get_group(x)) for filename, x in zip(gb.groups.keys(), gb.groups)]


'''
[create_tf_example()]

/ Descrpiton /
- Create tf.train.example object to write TFRecord file.

/ Parameters /
- group: dataframe to create tf.train.Example object
- path: image data path
- label_map_dict: label data

/ Retrun /
- tf.train.Example object about current dataframe
'''
def create_tf_example(group, path, label_map_dict):

    # read image file
    with tf.io.gfile.GFile(os.path.join(path, '{}'.format(group.filename)), 'rb') as fid:
        encoded_jpg = fid.read()
    encoded_jpg_io = io.BytesIO(encoded_jpg)
    image = Image.open(encoded_jpg_io)
    width, height = image.size

    # Data list for TFRecord
    filename = group.filename.encode('utf8')
    image_format = b'jpg'
    xmins = []
    xmaxs = []
    ymins = []
    ymaxs = []
    classes_text = []
    classes = []

    # Append coordinates of bounding box
    for index, row in group.object.iterrows():
        xmins.append(row['xmin'] / width)
        xmaxs.append(row['xmax'] / width)
        ymins.append(row['ymin'] / height)
        ymaxs.append(row['ymax'] / height)
        classes_text.append(row['class'].encode('utf8'))
        classes.append(class_text_to_int(row['class'], label_map_dict))

    # Record information of images to tf.train.example object
    tf_example = tf.train.Example(features=tf.train.Features(feature={
        'image/height': dataset_util.int64_feature(height),
        'image/width': dataset_util.int64_feature(width),
        'image/filename': dataset_util.bytes_feature(filename),
        'image/source_id': dataset_util.bytes_feature(filename),
        'image/encoded': dataset_util.bytes_feature(encoded_jpg),
        'image/format': dataset_util.bytes_feature(image_format),
        'image/object/bbox/xmin': dataset_util.float_list_feature(xmins),
        'image/object/bbox/xmax': dataset_util.float_list_feature(xmaxs),
        'image/object/bbox/ymin': dataset_util.float_list_feature(ymins),
        'image/object/bbox/ymax': dataset_util.float_list_feature(ymaxs),
        'image/object/class/text': dataset_util.bytes_list_feature(classes_text),
        'image/object/class/label': dataset_util.int64_list_feature(classes),
    }))

    # return tf.train.Example object
    return tf_example


'''
[CSVToTFrecord()]

/ Descrpiton /
- Convert CSV to TFRecord file.
- Iterate Train, Test, Valid folder by recording TFRecord file.

/ Parameters /
- tfrecord_path: TFRecod file location where stored.
- df_xml_to_csv: dataframe to convert TFRecord
    Dataframe that contains information of FILE_SET_NUM items
- labels_path: path of label_map.pbtxt
- idx: the order of the current folder.

/ Retrun /
nothing
'''
def CSVToTFrecord(tfrecord_path, df_xml_to_csv, cur_datset_folder_dir, labels_path, idx):
    label_map = label_map_util.load_labelmap(labels_path)
    label_map_dict = label_map_util.get_label_map_dict(label_map)

    # where to store TFRecord file
    tfrecord_path = os.path.join(tfrecord_path, "tfrecord" + idx.__str__())

    # TFRecord file generator
    tf_record_writer = tf.io.TFRecordWriter(tfrecord_path)

    # split dataframe to group
    grouped = split(df_xml_to_csv, 'filename')

    # iterate every group to store tf_exmaples to TFRecord file.
    for group in grouped:
        tf_example = create_tf_example(group, cur_datset_folder_dir, label_map_dict)

        # Write each tf_example obejct to TFRecord file.
        tf_record_writer.write(tf_example.SerializeToString())
    tf_record_writer.close()



if __name__ == '__main__':
    import argparse

    # argument settings.
    parser = argparse.ArgumentParser()
    # source dataset path
    parser.add_argument('--datasetdir', type=str,
                        help='You have to insert dataset directory')
    # resized dataset path
    parser.add_argument('--resizeddatasetdir', type=str,
                        help='You have to insert dataset directory')
    # source annotation path
    parser.add_argument('--annotationdir', type=str,
                        help='You have to insert dataset directory')
    # resized annotation path
    parser.add_argument('--resizedannotationdir', type=str,
                        help='You have to insert dataset directory')
    # width of image
    parser.add_argument('--datawidth', default=48, type=int,
                        help='Default data width is 48px')
    # height of image
    parser.add_argument('--dataheight', default=48, type=int,
                        help='Default data height is 48px')
    # label_map.pbtxt path
    parser.add_argument('--labelspath', type=str,
                        help='Path to the labels (.pbtxt) file.')


    # Set python argument to global FLAGS
    FLAGS, _ = parser.parse_known_args()

    src_dataset_dir = FLAGS.datasetdir
    dst_resized_dataset_dir = FLAGS.resizeddatasetdir

    src_annotation_dir = FLAGS.annotationdir
    dst_resized_annotation_dir = FLAGS.resizedannotationdir

    labels_path = FLAGS.labelspath

    # Resized image shape
    target_dim = (FLAGS.dataheight, FLAGS.datawidth)

    print('\n\n\n\n')

    # ##### RESIZING #####
    print('START RESIZING DATASET ...')
    Resizing(src_dir=src_dataset_dir, dst_dir=dst_resized_dataset_dir)
    print('END RESIZING DATASET ...')

    print('START RESIZING ANNOTATION ...')
    total_xml_df, xml_df_list = XmlToCSV(target_dim=target_dim,
                                        src_dir=src_annotation_dir,
                                        dst_dir=dst_resized_annotation_dir,
                                        src_dataset_dir=src_dataset_dir)
    print('END RESIZING ANNOTATION ...')


    print('\n\n\n\n')
    print('START CONVERTING TO TFRECROD ...')

    print(os.listdir(dst_resized_dataset_dir))

    # idx: index of 'dst_resized_dataset_dir' list
    for idx, cur_dataset_folder in enumerate(sorted(os.listdir(dst_resized_dataset_dir))):
        cur_dataset_folder_dir = os.path.join(dst_resized_dataset_dir, cur_dataset_folder)

        print(f'dst_resized_dataset_dir={dst_resized_dataset_dir}')
        print(f'cur_dataset_folder_dir={cur_dataset_folder_dir}')
        print(f'cur_dataset_folder={cur_dataset_folder}')
        print(f'cur_dataset_folder_dir={sorted(os.listdir(cur_dataset_folder_dir))}')

        # orderd by sort
        for idx, cur_data_folder in enumerate(sorted(os.listdir(cur_dataset_folder_dir))):
            CSVToTFrecord(dst_resized_annotation_dir, xml_df_list[idx],
                        os.path.join(cur_dataset_folder_dir, cur_data_folder), labels_path, idx)