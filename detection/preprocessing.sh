#!/bin/bash

SRC_DATASET_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Dataset'
RESIZED_DATASET_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Resized/Dataset'

SRC_ANNOTATION_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Annotations'
RESIZED_ANNOTATION_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset_02/Resized/Annotations'

LABELS_PATH='/home/gihkim/project/UAV_DATASET/label_map.pbtxt'

DATA_WIDTH=320
DATA_HEIGHT=320

python dataset-preprocessing.py --datasetdir $SRC_DATASET_DIR \
        --resizeddatasetdir $RESIZED_DATASET_DIR \
        --annotationdir $SRC_ANNOTATION_DIR \
        --resizedannotationdir $RESIZED_ANNOTATION_DIR \
        --dataheight $DATA_HEIGHT \
        --datawidth $DATA_WIDTH \
        --labelspath $LABELS_PATH
