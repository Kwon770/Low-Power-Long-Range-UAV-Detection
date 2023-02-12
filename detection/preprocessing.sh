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

# Python program to explain cv2.imshow() method

# # # importing cv2
# import cv2

# # path
# path = 'yoto10789.png'

# # Reading an image in default mode
# image = cv2.imread(path)

# # Window name in which image is displayed
# window_name = 'image'

# # Using cv2.imshow() method
# # Displaying the image
# cv2.imshow(window_name, image)

# # waits for user to press any key
# # (this is necessary to avoid Python kernel form crashing)
# cv2.waitKey(0)

# # closing all open windows
# cv2.destroyAllWindows()