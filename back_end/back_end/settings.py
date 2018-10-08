# -*-coding:utf-8-*-
"""
Django settings for back_end project.

Generated by 'django-admin startproject' using Django 1.11.15.

For more information on this file, see
https://docs.djangoproject.com/en/1.11/topics/settings/

For the full list of settings and their values, see
https://docs.djangoproject.com/en/1.11/ref/settings/
"""

import os

# Build paths inside the project like this: os.path.join(BASE_DIR, ...)

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

import sys

sys.path.insert(0, os.path.join(BASE_DIR, 'apps'))

# Quick-start development settings - unsuitable for production
# See https://docs.djangoproject.com/en/1.11/howto/deployment/checklist/

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = 'uzk_o#6l#bt*@(0+^v@z4*iut28&plfv)tqwec#2k-=6o5kn!z'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

ALLOWED_HOSTS = ['127.0.0.1', '10.9.19.139', 'localhost']


# Application definition
INSTALLED_APPS = [
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'rest_framework',
    'corsheaders',
    'rest_framework_swagger',

    'patientinformations.apps.PatientinformationsConfig',
    'alg_proxy.apps.AlgProxyConfig',
    'handle_pacs.apps.HandlePacsConfig',
    'upload_dcm.apps.UploadDcmConfig',
    'connect_image.apps.ConnectImageConfig'
]

MIDDLEWARE = [
    'django.middleware.security.SecurityMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'corsheaders.middleware.CorsMiddleware',
    'django.middleware.common.CommonMiddleware',
    # 'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
]

# --------------------------------------------------------------------------------
# CORS跨域
# CORS_ORIGIN_WHITELIST = (
#     '10.9.19.69:8000',
#     '127.0.0.1:8000',
# )
CORS_ORIGIN_ALLOW_ALL = True
# 跨域增加忽略
# 允许携带cookie
CORS_ALLOW_CREDENTIALS = True
CORS_ORIGIN_WHITELIST = (
    '*'
)

CORS_ALLOW_METHODS = (
    'DELETE',
    'GET',
    'OPTIONS',
    'PATCH',
    'POST',
    'PUT',
    'VIEW',
)

CORS_ALLOW_HEADERS = (
    'XMLHttpRequest',
    'X_FILENAME',
    'accept-encoding',
    'authorization',
    'content-type',
    'dnt',
    'origin',
    'user-agent',
    'x-csrftoken',
    'x-requested-with',
    'Pragma',
)

# --------------------------------------------------------------------------------
ROOT_URLCONF = 'back_end.urls'  # 总路由

TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': [os.path.join(BASE_DIR, 'back_end/templates')],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
        },
    },
]

WSGI_APPLICATION = 'back_end.wsgi.application'


# Database
# https://docs.djangoproject.com/en/1.11/ref/settings/#databases

# -------------------------------------------------------------------------------
# 连接数据库
DATABASES = {
    'default': {
        # 关联的数据库类型
        'ENGINE': 'django.db.backends.mysql',
        # 数据库名
        'NAME': 'ai_portal',
        # 数据库登陆用户名
        'USER': 'root',
        # 数据库登陆密码
        'PASSWORD': '111111',
        # 主机名
        'HOST': 'localhost',
        # 端口号
        'PORT': '3306',
        # 编码格式
        'default-character-set': 'utf8',
        'OPTIONS': {
         "init_command": "SET foreign_key_checks = 0;",
        },
        'default_transaction_isolation': 'read committed'
    }
}
# ---------------------------------------------------------------------------------------


# Password validation
# https://docs.djangoproject.com/en/1.11/ref/settings/#auth-password-validators

AUTH_PASSWORD_VALIDATORS = [
    {
        'NAME': 'django.contrib.auth.password_validation.UserAttributeSimilarityValidator',
    },
    {
        'NAME': 'django.contrib.auth.password_validation.MinimumLengthValidator',
    },
    {
        'NAME': 'django.contrib.auth.password_validation.CommonPasswordValidator',
    },
    {
        'NAME': 'django.contrib.auth.password_validation.NumericPasswordValidator',
    },
]


# Internationalization
# https://docs.djangoproject.com/en/1.11/topics/i18n/

LANGUAGE_CODE = 'zh-CN'  # 语言    # 'zh-hans'为python3的设置

TIME_ZONE = 'Asia/Shanghai'  # 时区

USE_I18N = True

USE_L10N = True

# USE_TZ = True

# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/1.11/howto/static-files/


STATIC_URL = '/static/'
STATIC_ROOT = os.path.join(BASE_DIR, 'static')
# =============================================================================
REST_FRAMEWORK = {

    'DEFAULT_RENDERER_CLASSES': (  # 默认响应渲染类

        'rest_framework.renderers.JSONRenderer',  # json渲染器

        'rest_framework.renderers.BrowsableAPIRenderer',  # 浏览API渲染器

    ),
    'DEFAULT_PAGINATION_CLASS': 'rest_framework.pagination.PageNumberPagination',
    'PAGE_SIZE': 5

}
# ===========================================================================================
