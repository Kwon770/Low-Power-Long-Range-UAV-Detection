# Data preprocessing module
- Overiew <br>

    This README is about how to preprocess dataset and annotation. <br>
    Before training our TensorFlow model, you must have to preprocess the dataset to resize specific iamge size. (We highly recommend less than 48x48x image).<br>
    Data preprocessing is runned separately local not in Colab.<br>
    You don't need any GPU device for running this code.<br>

    > !! If you don't want to run code. You can just download preprocessed data in this [link](https://drive.google.com/drive/folders/1Lz37Z6tDYc9uKrX8shi2bPANILqSFWtu?usp=share_link). <br>
    > Dataset: `Resized_48x48_grayscale.zip`

- (Option) TFRecord <br>

    We support XML to TFRecord function. <br>
    If you wnat to train model fastly. You can use TFRecord. <br>
    But our model training code not support TFRecord just CSV. <br>
    You have to chnage our code to use TFRecord.

<br>
<br>

# Setup

<br>

## 0. Dependencies
<hr>

You need to install bellow libraries to preprocess dataset.

```
cv2 4.5.5
    pip install opencv-python

Python 3.9.7
    https://www.python.org/downloads/

pandas 1.3.4
    pip install pandas

tensorflow 2.8.0
    pip install tensorflow
```




You have to clone `tensorflow/models` to project home dirtectoy <br>
It used to create TFRecord.

```
git clone https://github.com/tensorflow/models
```

<br>
<br>

## 1.  Prepare dataset
<hr>

- [Original Dataset](https://www.kaggle.com/datasets/dasmehdixtr/drone-dataset-uav) <br>
    This dataset has 1359 phtos and labeled data. <br>
    This dataset is labeled based on the "[Drone Dataset: Amateur Unmanned Air Vehicle Detection](https://data.mendeley.com/datasets/zcsj2g2m4c/4)" dataset.

- [Labeled dataset](https://www.kaggle.com/datasets/dasmehdixtr/drone-dataset-uav) support `jpg, png`
format.<br><br>

### 1-1. Labeled dataset file structure
You can download dataset in this link [the dataset](https://www.kaggle.com/datasets/dasmehdixtr/drone-dataset-uav) to train model.

- Original dataset file structure

    After you download [the dataset](https://www.kaggle.com/datasets/dasmehdixtr/drone-dataset-uav) the file structure will be below.

    ```
    archive
    ├── dataset_xml_format
    │   └── dataset_xml_format
    └── drone_dataset_yolo
        └── dataset_txt
    ```

    **Before running preprocessing code.** <br>
    You need to organize dataset like below structure. Or you can download organized dataset [link](https://drive.google.com/drive/folders/1Lz37Z6tDYc9uKrX8shi2bPANILqSFWtu?usp=share_link).


- Organized dataset file structure
    ```
    UAV_DATASET
    ├── Dataset
    │    ├─ train
    │    │   └─ image01.jpg
    │    │   └─ image02.jpg
    │    ├─ test
    │    │   └─ image01.jpg
    │    │   └─ image02.jpg
    │    └─ val
    │        └─ image01.jpg
    │        └─ image01.jpg
    └── Annotations
         ├─ train
         │   └─ image01.xml
         │   └─ image02.xml
         ├─ test
         │   └─ image01.xml
         │   └─ image02.xml
         └─ val
            └─ image01.xml
            └─ image02.xml
    ```


- Label map
    ```
    item {
    id: 1
    name: 'drone'
    }

    ```
- XML file structure
    ```
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
    ```

<br>

# Get strated
Plz follow these instructions.
<br>


## 1. Initialize preprocessing.sh
<hr>

- You have to set your data preprocessing enviornment.
    - `SRC_DATASET_PATH` : Your source dataset(not resized yet) path.
    - `RESIZED_DATASET_PATH` : Your source datset(not resized yet)is resized to specific data size.
    - `SRC_ANNOTATION_PATH` : Your label information about source dataset(not resized yet).
    - `RESIZED_ANNOTATION_PATH` : Your annotation files are updated to specific image size ratio.
    - If you want to run on our HW system, we highly recommend 48x48 or lower than 48x48.
        - `LABELS_PATH` : Your lable map path.
        - `DATA_WIDTH` : You can set image width.
        - `DATA_HEIGHT` : You can set image height.

- preprocessing.sh
    ```
    SRC_DATASET_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Dataset'
    RESIZED_DATASET_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Resized_48x48_grayscale/Dataset'

    SRC_ANNOTATION_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Annotations'
    RESIZED_ANNOTATION_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Resized_48x48_grayscale/Annotations'

    LABELS_PATH='/home/gihkim/project/UAV_DATASET/label_map.pbtxt'

    DATA_WIDTH=48
    DATA_HEIGHT=48

    python dataset-preprocessing.py --datasetdir $SRC_DATASET_DIR \
            --resizeddatasetdir $RESIZED_DATASET_DIR \
            --annotationdir $SRC_ANNOTATION_DIR \
            --resizedannotationdir $RESIZED_ANNOTATION_DIR \
            --dataheight $DATA_HEIGHT \
            --datawidth $DATA_WIDTH \
            --labelspath $LABELS_PATH
    ```
<br>
<br>


## 2. Run preprocessing code
<hr>
You can run preporcess.py with two options: `shell` or directly run `python`

- shell
    ```
    bash preprocessing.sh
    sh preprocessing.sh
    ```

- python
    ```
    python dataset-preprocessing.py --datasetdir $SRC_DATASET_PATH \
            --resizeddatasetdir $RESIZED_DATASET_PATH \
            --annotationdir $SRC_ANNOTATION_PATH \
            --resizedannotationdir $RESIZED_ANNOTATION_PATH \
            --dataheight $DATA_HEIGHT \
            --datawidth $DATA_WIDTH \
            --labelspath $LABELS_PATH
    ```
<br>
<br>



## 3. Result
<hr>

- You can get resized Dataset and Annotation at `RESIZED_DATASET_PATH` and `RESIZED_ANNOTATION_PATH`

```
    - 사용한 명령어
    make -f tensorflow/lite/micro/tools/make/Makefile TARGET=arduino TAGS="" generate_hello_world_arduino_library_zip

    tensorflow/lite/micro/tools/make/Makefile
```