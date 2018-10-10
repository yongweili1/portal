# load
加载体数据
## args
- seriesuid：序列id
- volume：体数据

## response
- success
- comment
- content

# show
获取一张图像数据（base64编码）
## args
- width：画布宽度（像素）
- height：画布高度（像素）
- display_view：画布显示的图像方位（transverse，coronal，saggital）

## response
- success
- comment
- content：base64编码图像数据
    - image data

# page
## args
- width：画布宽度（像素）
- height：画布高度（像素）
- focus_view：当前操作的画布的图像方位（transverse，coronal，saggital）
- delta：鼠标中键滚动偏移量

## response
- success
- comment
- content：base64编码图像数据
    - image data
