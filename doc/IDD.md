# Research Potal 前后端接口文档


---

- [Research Potal 前后端接口文档](#research-potal-%E5%89%8D%E5%90%8E%E7%AB%AF%E6%8E%A5%E5%8F%A3%E6%96%87%E6%A1%A3)
    - [接口列表](#%E6%8E%A5%E5%8F%A3%E5%88%97%E8%A1%A8)
    - [详细接口描述](#%E8%AF%A6%E7%BB%86%E6%8E%A5%E5%8F%A3%E6%8F%8F%E8%BF%B0)
        - [上传图像接口](#%E4%B8%8A%E4%BC%A0%E5%9B%BE%E5%83%8F%E6%8E%A5%E5%8F%A3)
        - [病人列表接口](#%E7%97%85%E4%BA%BA%E5%88%97%E8%A1%A8%E6%8E%A5%E5%8F%A3)
            - [病人列表请求(front to back)](#%E7%97%85%E4%BA%BA%E5%88%97%E8%A1%A8%E8%AF%B7%E6%B1%82front-to-back)
            - [病人列表(back to front)](#%E7%97%85%E4%BA%BA%E5%88%97%E8%A1%A8back-to-front)
            - [病人数据(back to front)](#%E7%97%85%E4%BA%BA%E6%95%B0%E6%8D%AEback-to-front)

---
## 接口列表
| 接口名称 |               URL               | 方式 | 备注 |
| -------- | ------------------------------- | ---- | ---- |
| 上传图像 | dicom/patinfos/                 | POST |      |
| 病人列表 | patientinformations/patinfolist | GET  |      |


----------
## 详细接口描述
### 上传图像接口
...
### 病人列表接口
#### 病人列表请求(front to back)
|    字段     |  类型  |     备注     |
| ----------- | ------ | ------------ |
| size        | string | 单页行数     |
| page        | string | 请求页数     |
| patientId   | string | 请求病人ID   |
| patientName | string | 请求病人姓名 |
| gender      | string | 请求性别     |
| modality    | string | 请求检查手段 |
| studyDate   | date   | 请求检查时间 |

#### 病人列表(back to front)
|       字段       |  类型  |   备注   |
| ---------------- | ------ | -------- |
| content[]        |        | 病人数据 |
| totalPages       | number | 总页数   |
| totalElements    | number | 总记录数 |
| size             | number | 单页行数 |
| number           | number | 当前页数 |
| numberOfElements | number | 该页行数 |

#### 病人数据(back to front)
|       字段       |  类型  |   备注   |
| ---------------- | ------ | -------- |
| patientId        | string | 病人ID   |
| patientName      | string | 病人姓名 |
| gender           | string | 性别     |
| modality         | string | 检查手段 |
| studyDescription | string | 检查概述 |
| studyDate        | string | 检查时间 |
