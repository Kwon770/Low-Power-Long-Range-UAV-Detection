#!/bin/bash

SRC_DATASET_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset/Dataset'
RESIZED_DATASET_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset/Resized_48x48_grayscale/Dataset'

SRC_ANNOTATION_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset/Annotations'
RESIZED_ANNOTATION_DIR='/home/gihkim/project/UAV_DATASET/labeled_dataset/Resized_48x48_grayscale/Annotations'

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
