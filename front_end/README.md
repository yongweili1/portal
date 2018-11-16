# uAI-Research-ui

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 1.6.7.

## Development server

Run `ng serve` for a dev server. Navigate to `http://localhost:4200/`. The app will automatically reload if you change any of the source files.

## Code scaffolding

Run `ng generate component component-name` to generate a new component. You can also use `ng generate directive|pipe|service|class|guard|interface|enum|module`.

## Build

Run `ng build` to build the project. The build artifacts will be stored in the `dist/` directory. Use the `-prod` flag for a production build.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via [Protractor](http://www.protractortest.org/).

## Further help

To get more help on the Angular CLI use `ng help` or go check out the [Angular CLI README](https://github.com/angular/angular-cli/blob/master/README.md).

##test


# 代码结构
[angular-starter](https://github.com/gdi2290/angular-starter)

```
front_end/
 ├──config/                        * our configuration
 |   ├──build-utils.js             * common config and shared functions for prod and dev
 |   ├──config.common.json         * config for both environments prod and dev such title and description of index.html
 |   ├──config.dev.json            * config for devevlopment environment
 |   ├──config.prod.json           * config for production environment 
 │   │                              (note: you can load your own config file, just set the evn ANGULAR_CONF_FILE with the path of your own file)
 |   ├──helpers.js                 * helper functions for our configuration files
 |   ├──spec-bundle.js             * ignore this magic that sets up our Angular testing environment
 |   ├──karma.conf.js              * karma是单元测试的执行器，karma.conf.js是karma的配置文件
 |   ├──protractor.conf.js         * 也是一个做自动化测试的配置文件end-to-end
 │   ├──webpack.common.js          * common tasks for webpack build process shared for dev and prod
 │   ├──webpack.dev.js             * our development webpack config
 │   ├──webpack.prod.js            * our production webpack config
 │   └──webpack.test.js            * our testing webpack config
 │
 ├──src/                           * our source files that will be compiled to javascript
 |   ├──main.browser.ts            * our entry file for our browser environment
 │   │
 |   ├──index.html                 * 整个应用的根html，程序启动就是访问这个页面
 |   ├──main.ts                    * 整个项目的入口点，Angular通过这个文件来启动项目
 |   ├──polyfills.ts               * 主要是用来导入一些必要库，为了让Angular能正常运行在老版本下
 |   ├──styles.css                 * 主要是放一些全局的样式 
 |   ├──tsconfig.app.json          * TypeScript编译器的配置,添加第三方依赖的时候会修改这个文件
 |   ├──tsconfig.spec.json
 |   ├──test.ts                    * 也是自动化测试用的
 |   ├──typings.d.ts
 │   │
 │   ├──app/                       * WebApp: folder
 │   │   ├──components/            * 按业务模块进行分类，和业务耦合比较紧密的组件放在各自的父组件中，否则放在common组件中
 │   │   │   ├──navi/
 │   │   │   ├──data/
 │   │   │   │   ├──file_upload/
 │   │   │   │   └──pacs/
 │   │   │   ├──pa/
 │   │   │   ├──contouring/
 │   │   │   ├──status/
 │   │   │   ├──settings/
 │   │   │   │   ├──user_setting/
 │   │   │   │   └──sys_setting/
 │   │   │   └──common/            * 公共组件，用户自定义的一些控件
 │   │   │
 │   │   ├──service/               * 业务相关的service，按照组件进行分类
 │   │   ├──models/                * 业务相关的models
 │   │   ├──utilities/             * 工具类
 │   │   │ 
 │   │   ├──app.component.spec.ts  * a simple test of components in app.component.ts
 │   │   ├──app.e2e.ts             * a simple end-to-end test for /
 │   │   ├──app.component.ts       * a simple version of our App component components
 │   │   ├──app.module.ts          * a simple version of our App module
 │   │   └──app.routes.ts          * a simple version of our App routes
 │   │
 │   ├──assets/                    * static assets are served here
 │   │   ├──icon/                  * our list of icons from www.favicon-generator.org
 │   │   ├──service-worker.js      * ignore this. Web App service worker that's not complete yet
 │   │   ├──robots.txt             * for search engines to crawl your website
 │   │   └──humans.txt             * for humans to know who the developers are
 │   │ 
 │   └──third-party/               * 三方库
 │       ├──clipper/             
 │       └──bootstrap/
 │
 │
 ├──tslint.json                    * typescript lint config
 ├──typedoc.json                   * typescript documentation generator
 ├──tsconfig.json                  * typescript config used outside webpack
 ├──tsconfig.webpack.json          * config that webpack uses for typescript
 ├──package.json                   * 这是一个标准的npm工具的配置文件，这个文件里面列出了该应用程序所使用的第三方依赖包。
 │                                   实际上我们在新建项目的时候，等了半天就是在下载第三方依赖包。
 │                                   下载完成后会放在node_modules这个目录中，后期我们可能会修改这个文件。
 ├──Dockerfile                     * docker文件
 ├──Dockerfile-dev                 * docker文件（开发版）
 └──webpack.config.js              * webpack main configuration file

```