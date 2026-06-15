import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.layers import Dense, GlobalAveragePooling2D
from tensorflow.keras.models import Model
from tensorflow.keras.callbacks import ReduceLROnPlateau, EarlyStopping
import matplotlib.pyplot as plt
import os

# 1. 基础配置
DATASET_DIR = r"D:\python_project\dataset"
IMG_SIZE = (224, 224)
BATCH_SIZE = 16  # R7000P 跑 batch_size=16 非常轻松
EPOCHS = 15      # 跑15轮足够画出漂亮的曲线

def build_dataset():
    # 引入数据增强，提升模型泛化能力
    datagen = ImageDataGenerator(
        rescale=1./255,
        rotation_range=20,
        width_shift_range=0.2,
        height_shift_range=0.2,
        horizontal_flip=True,
        validation_split=0.2 # 划分 80% 训练集，20% 验证集
    )

    train_generator = datagen.flow_from_directory(
        DATASET_DIR,
        target_size=IMG_SIZE,
        batch_size=BATCH_SIZE,
        class_mode='categorical',
        subset='training'
    )

    validation_generator = datagen.flow_from_directory(
        DATASET_DIR,
        target_size=IMG_SIZE,
        batch_size=BATCH_SIZE,
        class_mode='categorical',
        subset='validation'
    )
    
    return train_generator, validation_generator

def build_model(num_classes):
    # 加载 MobileNetV2 底层权重，丢弃顶层全连接层
    base_model = MobileNetV2(weights='imagenet', include_top=False, input_shape=(224, 224, 3))
    
    # 冻结底层，只训练我们自己的分类器
    base_model.trainable = False 
    
    x = base_model.output
    x = GlobalAveragePooling2D()(x)
    x = Dense(128, activation='relu')(x)
    predictions = Dense(num_classes, activation='softmax')(x)
    
    model = Model(inputs=base_model.input, outputs=predictions)
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001), 
                  loss='categorical_crossentropy', 
                  metrics=['accuracy'])
    return model

def plot_history(history):
    # 绘制具备科研感的训练折线图
    acc = history.history['accuracy']
    val_acc = history.history['val_accuracy']
    loss = history.history['loss']
    val_loss = history.history['val_loss']
    epochs_range = range(len(acc))

    plt.figure(figsize=(12, 5))
    plt.subplot(1, 2, 1)
    plt.plot(epochs_range, acc, label='Training Accuracy')
    plt.plot(epochs_range, val_acc, label='Validation Accuracy')
    plt.legend(loc='lower right')
    plt.title('Training and Validation Accuracy')

    plt.subplot(1, 2, 2)
    plt.plot(epochs_range, loss, label='Training Loss')
    plt.plot(epochs_range, val_loss, label='Validation Loss')
    plt.legend(loc='upper right')
    plt.title('Training and Validation Loss')
    
    plt.savefig('training_history.png', dpi=300) # 保存高清图供论文使用
    print("✅ 训练折线图已保存为 training_history.png")
    plt.show()

if __name__ == "__main__":
    print("正在加载数据集...")
    train_gen, val_gen = build_dataset()
    
    print(f"检测到类别数: {train_gen.num_classes} ({list(train_gen.class_indices.keys())})")
    
    model = build_model(train_gen.num_classes)
    
    # 设置高级回调函数
    callbacks = [
        # 当验证集准确率不再提升时，自动减小学习率
        ReduceLROnPlateau(monitor='val_accuracy', patience=3, factor=0.5, verbose=1),
        # 如果连续5轮没有提升，提前停止，防止过拟合
        EarlyStopping(monitor='val_accuracy', patience=5, restore_best_weights=True)
    ]
    
    print("开始训练模型 (预计需要几分钟)...")
    history = model.fit(
        train_gen,
        validation_data=val_gen,
        epochs=EPOCHS,
        callbacks=callbacks
    )
    
    # 保存权重文件供 app.py 使用
    model.save('leaf_model.h5')
    print("✅ 模型已保存为 leaf_model.h5")
    
    # 画图
    plot_history(history)