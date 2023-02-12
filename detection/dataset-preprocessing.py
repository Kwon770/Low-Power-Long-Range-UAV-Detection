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
[Dataset, Annotation, xml file naming format]
    Dataset
    └─ train
        └─ image01.jpg
        └─ image01.jpg
    └─ val
        └─ image01.jpg
        └─ image01.jpg
    └─ test
        └─ image01.jpg
        └─ image01.jpg


    Annotations
    └─ train
        └─ image01.xml
        └─ image01.xml
    └─ val
        └─ image01.xml
        └─ image01.xml
    └─ test
        └─ image01.xml
        └─ image01.xml

    [XML structure]
    <annotation>
        <folder>images</folder>
        <filename>0_IMG_2476.JPG</filename>
        <size>
            <width>4032</width>
            <height>3024</height>
            <depth>3</depth>
        </size>
        <object>
            <name>uav</name>
            <pose>Unspecified</pose>
            <truncated>0</truncated>
            <difficult>0</difficult>
            <bndbox>
                <xmin>2020</xmin>
                <ymin>1345</ymin>
                <xmax>2208</xmax>
                <ymax>1509</ymax>
            </bndbox>
        </object>
    </annotation>


    xmin: x of object bounding box (left up)
    ymin: y of object bounding box (left up)
    xmax: x of object bounding box (right bottom)
    ymax: y of object bounding box (right bottom)

'''


'''
[convert a image to specific size]

target_dim: to resize dimension
src_path: source file path
'''
def ConvertToSpecificSize(target_dim, src_path):
    src_image = cv2.imread(src_path, cv2.IMREAD_UNCHANGED)

    # resize image
    resized = cv2.resize(src_image, target_dim, interpolation=cv2.INTER_AREA)

    return (resized)


'''
[convert xml to csv]


/ Descrpiton /
convert annotated file (xml) to CSV file foramt
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
def XmlToCSV(target_dim, src_dir, dst_dir):
    import xml.etree.ElementTree as ET

    if not(os.path.isdir(dst_dir)):
        os.makedirs(dst_dir)

    # for dataframe parameters
    xml_elements_list = [] # data
    column_name = ['filename', 'width', 'height',
                    'class', 'xmin', 'ymin', 'xmax', 'ymax'] # classes

    file_id = 0
    directory_id = 0

    # for return
    ret_xml_df = pd.DataFrame(columns=column_name)

    # It will stored [Train_dataframe, Valid_dataframe, Test_dataframe]
    ret_xml_df_list = []


    # iterate each annotatoin folders
    for folder_name in os.listdir(path=src_dir):
        print(f'folder_name={folder_name}')
        cur_folder_dir = os.path.join(src_dir, folder_name)
        # resized_dir = os.path.join(dst_dir, folder_name)
        prefix_resized_dir = os.path.join(dst_dir, folder_name)
        resized_dir = os.path.join(prefix_resized_dir, "annotation" + directory_id.__str__())

        for xml_file_name in os.listdir(path=cur_folder_dir):


            # read xml file and get data
            cur_file_dir = os.path.join(cur_folder_dir, xml_file_name)

            tree = ET.parse(cur_file_dir)

            # bellow var is xml tag.
            root_tag = tree.getroot()
            file_name = root_tag.find('filename').text
            size_tag = root_tag.find('size')
            object_tag = root_tag.find('object')

            src_width = int(size_tag.find('width').text)
            src_height = int(size_tag.find('height').text)

            # (height scale, width scale) = target_dim / src_shape
            scale_factor = (target_dim[0] / src_height, target_dim[1] / src_width)
            class_name = object_tag.find('name').text

            # coordinate (x, y)
            # order: 'xmin', 'ymin', 'xmax', 'ymax'
            bounding_box_tag = object_tag.find('bndbox')

            # shape (height, width)
            # width = x, height = y

            # (filename, width, height, class, xmin, ymin, xmax, ymax)
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
                    print(f'resized_dir={resized_dir}')
                    os.makedirs(resized_dir)

                # create xml data frame for each xml folders
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

        # clear buffer
        if len(xml_elements_list) > 0:
            print(f'check buffer={len(xml_elements_list)}')
            print(f'file_id={file_id}')
            if not(os.path.isdir(resized_dir)):
                print(f'resized_dir={resized_dir}')
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


def Resizing(src_dir, dst_dir):
    target_dim = (FLAGS.dataheight, FLAGS.datawidth)
    file_id = 0
    directory_id = 0

    # iterate each annotatoin folders
    for folder_name in os.listdir(path=src_dir):
        cur_folder_dir = path=os.path.join(src_dir, folder_name)
        resized_dir = os.path.join(dst_dir, folder_name, "data" + directory_id.__str__())
        for file_name in os.listdir(cur_folder_dir):
            cur_file_dir = os.path.join(cur_folder_dir, file_name)

            # checking if it is a file
            if os.path.isfile(cur_file_dir):
                _, file_expansion = os.path.splitext(file_name)
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
                            os.makedirs(resized_dir)
                        print(resized_dir)

                    cv2.imwrite(os.path.join(tmp_resized_dir, file_name), resized_image)
                    file_id += 1


import io
import tensorflow as tf
from PIL import Image

# [CHECK] current python file must be in '/content/gdrive/MyDrive/models/research'
from models.research.object_detection.utils import dataset_util, label_map_util
from collections import namedtuple


def class_text_to_int(row_label, label_map_dict):
    return label_map_dict[row_label]


def split(df, group):
    data = namedtuple('data', ['filename', 'object'])
    gb = df.groupby(group)
    return [data(filename, gb.get_group(x)) for filename, x in zip(gb.groups.keys(), gb.groups)]


def create_tf_example(group, path, label_map_dict):
    with tf.io.gfile.GFile(os.path.join(path, '{}'.format(group.filename)), 'rb') as fid:
        encoded_jpg = fid.read()
    encoded_jpg_io = io.BytesIO(encoded_jpg)
    image = Image.open(encoded_jpg_io)
    width, height = image.size

    filename = group.filename.encode('utf8')
    image_format = b'jpg'
    xmins = []
    xmaxs = []
    ymins = []
    ymaxs = []
    classes_text = []
    classes = []

    for index, row in group.object.iterrows():
        xmins.append(row['xmin'] / width)
        xmaxs.append(row['xmax'] / width)
        ymins.append(row['ymin'] / height)
        ymaxs.append(row['ymax'] / height)
        classes_text.append(row['class'].encode('utf8'))
        classes.append(class_text_to_int(row['class'], label_map_dict))

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
    return tf_example


# iterate Train, Test, Valid
def CSVToTFrecord(tfrecord_path, df_xml_to_csv, cur_datset_folder_dir, labels_path, idx):
    label_map = label_map_util.load_labelmap(labels_path)
    label_map_dict = label_map_util.get_label_map_dict(label_map)

    tfrecord_path = os.path.join(tfrecord_path, idx.__str__())
    print(f'tfrecord_path={tfrecord_path}')
    tf_record_writer = tf.io.TFRecordWriter(tfrecord_path)

    grouped = split(df_xml_to_csv, 'filename')

    for group in grouped:
        tf_example = create_tf_example(group, cur_datset_folder_dir, label_map_dict)
        tf_record_writer.write(tf_example.SerializeToString())
    tf_record_writer.close()
    print(f'Successfully created the TFRecord file: {tfrecord_path}')



if __name__ == '__main__':
    import argparse

    # argument settings.
    parser = argparse.ArgumentParser()
    parser.add_argument('--datasetdir', type=str,
                        help='You have to insert dataset directory')
    parser.add_argument('--resizeddatasetdir', type=str,
                        help='You have to insert dataset directory')
    parser.add_argument('--annotationdir', type=str,
                        help='You have to insert dataset directory')
    parser.add_argument('--resizedannotationdir', type=str,
                        help='You have to insert dataset directory')
    parser.add_argument('--datawidth', default=320, type=int,
                        help='Default data width is 320px')
    parser.add_argument('--dataheight', default=320, type=int,
                        help='Default data height is 320px')
    parser.add_argument('--labelspath', type=str,
                        help='Path to the labels (.pbtxt) file.')

    FLAGS, _ = parser.parse_known_args()

    # init arguments
    # [TODO] Exception handling
    # wrong argument format

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
    total_xml_df, xml_df_list = XmlToCSV(target_dim=target_dim, src_dir=src_annotation_dir, dst_dir=dst_resized_annotation_dir)
    print('END RESIZING ANNOTATION ...')

    for idx, cur_dataset_folder in enumerate(os.listdir(dst_resized_dataset_dir)):
        cur_dataset_folder_dir = os.path.join(dst_resized_dataset_dir, cur_dataset_folder)

        print(f'dst_resized_dataset_dir={dst_resized_dataset_dir}')
        print(f'cur_dataset_folder_dir={cur_dataset_folder_dir}')

        for idx, cur_data_folder in enumerate(os.listdir(cur_dataset_folder_dir)):
            CSVToTFrecord(dst_resized_annotation_dir, xml_df_list[idx],
                        os.path.join(cur_dataset_folder_dir, cur_data_folder), labels_path, idx)