from views import load_volume

data = """session: "127.0.0.1"
server_name: "image"
command: "load"
content {
  params: "{\"seriesuid\": \"1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373\"}"
}
"""
volumepath = r"D:\volume\SHEN YU-Thorax^10_ZRY_LDCT_Head_first (Adult)-CT.nii.gz"
result = load_volume(data)
from views import visit_image_server
data = """�   
	127.0.0.1imageshow"�
�{"width": "400", "focus_view": "", "display_view": "all", "seriesuid": "1.3.12.2.1107.5.1.4.64606.30000018051006052134700006373", "height": "400"}"""
result = visit_image_server(data)
