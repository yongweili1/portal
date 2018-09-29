# Themes
- lightness：亮色
- darkness：暗色

# 样式less源文件
- table 表格
- form 表单
- tree 树
- dailog 对话框
- dashboard
- common 其它
- variable theme变量
- echart echart主题

# 编译 build.bat
npm install -g less

lessc lightness\my\theme.less > lightness\my\theme.css
type lightness\primeng\theme.css > lightness\theme.css
type lightness\my\theme.css >> lightness\theme.css

lessc darkness\my\theme.less > darkness\my\theme.css
type darkness\primeng\theme.css > darkness\theme.css
type darkness\my\theme.css >> darkness\theme.css

lessc custom\my\theme.less > custom\my\theme.css
type custom\primeng\theme.css > custom\theme.css
type custom\my\theme.css >> custom\theme.css