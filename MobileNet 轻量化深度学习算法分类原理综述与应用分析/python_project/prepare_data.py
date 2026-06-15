import os
import shutil
import random

# ================= 配置区 =================
# 数据源路径 (桌面解压的目录)
HEALTHY_SOURCE_DIR = r"D:\Desktop\tomatodata\Tomato___healthy"
SICK_SOURCE_DIR = r"D:\Desktop\tomatodata\Tomato___Late_blight"

# 项目目标数据集路径
TARGET_DIR = r"D:\python_project\dataset"

# 抽取数量
SAMPLE_SIZE = 100
# ==========================================

def create_and_sample():
    print("开始自动抽取数据集...")
    
    # 1. 处理健康叶片 (Healthy_Leaf)
    healthy_target = os.path.join(TARGET_DIR, "Healthy_Leaf")
    os.makedirs(healthy_target, exist_ok=True) # 如果文件夹不存在会自动创建
    
    if os.path.exists(HEALTHY_SOURCE_DIR):
        all_healthy = [f for f in os.listdir(HEALTHY_SOURCE_DIR) if f.endswith(('.jpg', '.JPG', '.png', '.jpeg'))]
        sampled_healthy = random.sample(all_healthy, min(SAMPLE_SIZE, len(all_healthy)))
        for img in sampled_healthy:
            shutil.copy(os.path.join(HEALTHY_SOURCE_DIR, img), os.path.join(healthy_target, f"healthy_{img}"))
        print(f"✅ 成功抽取 {len(sampled_healthy)} 张健康叶片。")
    else:
        print(f"⚠️ 找不到健康叶片源文件夹: {HEALTHY_SOURCE_DIR}")

    # 2. 处理病害叶片 (Sick_Leaf)
    sick_target = os.path.join(TARGET_DIR, "Sick_Leaf")
    os.makedirs(sick_target, exist_ok=True)
    
    if os.path.exists(SICK_SOURCE_DIR):
        all_sick = [f for f in os.listdir(SICK_SOURCE_DIR) if f.endswith(('.jpg', '.JPG', '.png', '.jpeg'))]
        sampled_sick = random.sample(all_sick, min(SAMPLE_SIZE, len(all_sick)))
        for img in sampled_sick:
            shutil.copy(os.path.join(SICK_SOURCE_DIR, img), os.path.join(sick_target, f"sick_{img}"))
        print(f"✅ 成功抽取 {len(sampled_sick)} 张病害图片。")
    else:
        print(f"⚠️ 找不到病害文件夹: {SICK_SOURCE_DIR}")
            
    print(f"\n🎉 数据集准备完毕！请检查 D:\\python_project\\dataset 目录。")

if __name__ == "__main__":
    create_and_sample()