# i18n json 规范
1. json数据不超过三个层级，比如base.user.title
2. 同一个页面下文本项，在一个json层级下
3. 数据表类，如base.user/sys-module/config/locales ...， 可以被多个页面引用
4. 页面类，如base.log/change-password/update-profile ...，只可被当前页面引用
