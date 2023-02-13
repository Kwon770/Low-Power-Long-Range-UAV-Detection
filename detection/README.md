# Data preprocessing moduel


## 0. File structure
- Dataset
    ```
    [Dataset]
        Dataset
        └─ train
            └─ image01.jpg
            └─ image01.jpg
        └─ test
            └─ image01.jpg
            └─ image01.jpg
        └─ val
            └─ image01.jpg
            └─ image01.jpg
    ```
- Annotation
    ```
        Annotations
        └─ train
            └─ image01.xml
            └─ image01.xml
        └─ test
            └─ image01.xml
            └─ image01.xml
        └─ val
            └─ image01.xml
            └─ image01.xml
    ```

- XML
    ```
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
    ```

## 1. Initialize preprocessing.sh

- preprocessing.sh
    ```
    SRC_DATASET_DIR='your source dataset directory'
    RESIZED_DATASET_DIR='your resized dataset directory'

    SRC_ANNOTATION_DIR='your source annotation(information about labeled data) directory'
    RESIZED_ANNOTATION_DIR='your resized annotation(information about labeled data) directory'

    LABELS_PATH='your label map directory'

    <!-- must be integer -->
    DATA_WIDTH='data width'
    DATA_HEIGHT='data width'
    ```

## 2. Run

- bash
    ```
    bash preprocessing.sh
    ```


- python
    ```
    python dataset-preprocessing.py --datasetdir $SRC_DATASET_DIR \
            --resizeddatasetdir $RESIZED_DATASET_DIR \
            --annotationdir $SRC_ANNOTATION_DIR \
            --resizedannotationdir $RESIZED_ANNOTATION_DIR \
            --dataheight $DATA_HEIGHT \
            --datawidth $DATA_WIDTH \
            --labelspath $LABELS_PATH
    ```