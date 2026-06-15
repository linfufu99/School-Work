import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageTk
import numpy as np
import tensorflow as tf
import os

# 1. 基础配置
MODEL_PATH = 'leaf_model.h5'
IMG_SIZE = (224, 224)
# 类别映射（必须与训练时的文件夹顺序一致：Healthy_Leaf=0, Sick_Leaf=1）
CLASS_NAMES = ['健康叶片 (Healthy)', '病害叶片 (Sick)']

class CropApp:
    def __init__(self, root):
        self.root = root
        self.root.title("基于 MobileNetV2 的农作物病害监测系统 V2.0")
        self.root.geometry("600x750")
        
        # 加载模型
        try:
            self.model = tf.keras.models.load_model(MODEL_PATH)
            print("✅ 模型加载成功")
        except Exception as e:
            messagebox.showerror("错误", f"模型加载失败: {e}")
            self.root.destroy()

        self.create_widgets()

    def create_widgets(self):
        # 标题
        self.title_label = tk.Label(self.root, text="农作物病害 AI 识别终端", font=("微软雅黑", 22, "bold"))
        self.title_label.pack(pady=20)

        # 图片预览区域
        self.image_canvas = tk.Label(self.root, text="请先上传叶片照片", bg="#f0f0f0", width=40, height=15, relief="sunken")
        self.image_canvas.pack(pady=10)

        # 预测结果展示区
        self.res_frame = tk.Frame(self.root)
        self.res_frame.pack(pady=20)
        
        self.result_text = tk.Label(self.res_frame, text="检测结果：等待输入", font=("微软雅黑", 18))
        self.result_text.pack()
        
        self.conf_text = tk.Label(self.res_frame, text="可靠性(置信度)：--", font=("微软雅黑", 12), fg="gray")
        self.conf_text.pack()

        # 按钮区域
        self.btn_upload = tk.Button(self.root, text=" 📂 选择叶片照片 ", command=self.upload_and_predict, 
                                   font=("微软雅黑", 14), bg="#2196F3", fg="white", padx=20, pady=10)
        self.btn_upload.pack(pady=20)

        # 状态栏
        self.status = tk.Label(self.root, text="系统就绪 | 算法核心：MobileNetV2", bd=1, relief="sunken", anchor="w")
        self.status.pack(side="bottom", fill="x")

    def upload_and_predict(self):
        file_path = filedialog.askopenfilename(filetypes=[("Image files", "*.jpg *.jpeg *.png")])
        if not file_path:
            return

        # 1. 预览图片
        img = Image.open(file_path)
        img_display = img.resize((300, 300))
        img_tk = ImageTk.PhotoImage(img_display)
        self.image_canvas.config(image=img_tk, text="")
        self.image_canvas.image = img_tk

        # 2. 预处理
        img_input = img.resize(IMG_SIZE)
        img_array = np.array(img_input) / 255.0
        img_array = np.expand_dims(img_array, axis=0)

        # 3. 推理
        predictions = self.model.predict(img_array)
        score = predictions[0]
        class_idx = np.argmax(score)
        confidence = score[class_idx] * 100

        # 4. 更新 UI
        res_str = CLASS_NAMES[class_idx]
        color = "#4CAF50" if class_idx == 0 else "#F44336" # 健康绿色，病害红色
        
        self.result_text.config(text=f"检测结果：{res_str}", fg=color)
        self.conf_text.config(text=f"AI 置信度：{confidence:.2f}%")
        self.status.config(text=f"分析完成：{os.path.basename(file_path)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = CropApp(root)
    root.mainloop()