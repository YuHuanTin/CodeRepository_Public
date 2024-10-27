import cv2
import mss
import numpy as np
import win32gui
from time import sleep


# THIS PROJECT IS FOR SKLAND'S MINIGAME 20241026
# Phone H * W = 793 * 358 Using QtScrcpy
# WARNING: This project is for testing only. Do not use it in production. 
# May result will be wrong.

### LIBRARY ###
# mss
# opencv-python
# win32gui
# numpy
def template_matching(image_path, template_path, threshold=0.8, method=cv2.TM_CCOEFF_NORMED):
    img = cv2.imread(image_path)
    template = cv2.imread(template_path)

    if img is None or template is None:
        print("无法读取图像或模板。请检查路径是否正确。")
        return []

    h, w, channels = template.shape
    res = cv2.matchTemplate(img, template, method)

    loc = np.where(res >= threshold)
    results = []
    for pt in zip(*loc[::-1]):  # Switch collumns and rows
        results.append((pt[0], pt[1], res[pt[1], pt[0]]))

    return results


def filter_close_centers_efficient(centers, min_distance=10):
    """
    使用 itertools 更高效地过滤掉彼此太近的中心点。
    """
    filtered_centers = []
    for center in centers:
        if not any(((center[0] - other[0]) ** 2 + (center[1] - other[1]) ** 2) ** 0.5 < min_distance for other in
                   filtered_centers):
            filtered_centers.append(center)
    return filtered_centers


def draw_matches(image_path, results, template_w, template_h):
    img_rgb = cv2.imread(image_path)
    centers = []
    for pt in results:
        cv2.rectangle(img_rgb, pt, (pt[0] + template_w, pt[1] + template_h), (0, 0, 255), 2)
        center_x = pt[0] + template_w // 2
        center_y = pt[1] + template_h // 2
        centers.append((center_x, center_y))

    centers = filter_close_centers_efficient(centers)

    for center in centers:
        cv2.circle(img_rgb, center, 5, (0, 0, 255), -1)

    print(f"找到 {len(centers)} 个中心点: {centers}")
    return centers


def get_centers(image_path, template_path):
    results = template_matching(image_path, template_path, threshold=0.91)

    if results:
        template = cv2.imread(template_path)
        h, w, channels = template.shape  # 获取彩色模板的尺寸
        print(f"找到 {len(results)} 个匹配项:")
        for pt in results:
            print(f"左上角坐标: ({pt[0]}, {pt[1]}), 匹配值: {pt[2]}")
        return draw_matches(image_path, [r[:2] for r in results], w, h)
    else:
        print("未找到匹配项。")


def screenshot_window(window_title=None):
    """
    从指定窗口标题的窗口截取图像。如果没有提供标题，则截取活动窗口。

    Args:
        window_title: 窗口的标题。如果为 None，则截取活动窗口。

    Returns:
        numpy 数组形式的图像，如果找不到窗口则返回 None。
    """
    if window_title:
        hwnd = win32gui.FindWindow(None, window_title)
    else:
        hwnd = win32gui.GetForegroundWindow()

    if not hwnd:
        print("找不到窗口！")
        return None

    # 获取窗口在屏幕上的矩形坐标
    x, y, right, bottom = win32gui.GetWindowRect(hwnd)
    width = right - x
    height = bottom - y

    with mss.mss() as sct:
        monitor = {"top": y, "left": x, "width": width, "height": height}
        sct_img = sct.grab(monitor)

        # 将截图转换为 numpy 数组
        img = np.array(sct_img)
        return img


while True:
    sleep(5)
    screenshot = screenshot_window('Phone-10AC4X1GNT000Q3')

    cv2.imwrite('test111.jpg', screenshot)
    # 使用示例
    image_path = 'test111.jpg'  # 请替换为你的图像路径

    # 获取红色和蓝色中心点
    center_red = get_centers(image_path, 'red.jpg')
    center_blue = get_centers(image_path, 'blue.jpg')
    centers = []
    if not center_red or not center_blue:
        continue
    for center in center_blue:
        centers.append((center[0] + 5, center[1] - 174))

    for center in center_red:
        centers.append(center)

    print('total: ', len(centers), 'red: ', len(center_red), 'blue: ', len(center_blue))
    centers = filter_close_centers_efficient(centers)
    print(len(centers), centers)

    img = cv2.imread(image_path)
    for center in centers:
        cv2.circle(img, center, 5, (0, 0, 255), -1)
    cv2.imwrite('output_image.jpg', img)
