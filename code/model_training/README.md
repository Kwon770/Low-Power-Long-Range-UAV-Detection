# Model training
We designed simple and small drone detection model by using TensorFlow. <br>
This code is running on <b>the Google Colab</b>.

<br>

# Stepup

## 0. Dependencies
```
pandas version= 1.3.5
    pip install pandas

numpy version= 1.22.4
    pip install numpy

tensorflow version= 2.2.0
    pip install -U --pre tensorflow=='2.2.0'

matplotlib= 3.5.3
    pip install matplotlib

PIL version= 7.1.2
```

## 0. Prepare dataset
  First you must preprocess dataset and then run this code. <br>
  Plz make your dataset structure like below. <br>
  Or if you want custom structure, you have to change several code. <br>

- Dataset folder structure.
  ```
    GooglDrive
    └─ test
        └─ {Dataset_name}.zip
  ```

<br>

# Get started

- plz run the `model_training.ipynb` file on the Colab.