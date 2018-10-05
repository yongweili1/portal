# Research Potal 前后端接口文档


---


<!-- code_chunk_output -->

* [Research Potal 前后端接口文档](#research-potal-前后端接口文档)
	* [接口列表](#接口列表)
	* [详细接口描述](#详细接口描述)
		* [病人列表接口](#病人列表接口)
			* [病人列表请求](#病人列表请求)
			* [病人列表](#病人列表)
			* [病人数据](#病人数据)


---
## 接口列表
| 接口名称 |               URL               | 方式 | 备注 |
| -------- | ------------------------------- | ---- | ---- |
| 病人列表 | patientinformations/patinfolist | GET  |      |


----------
## 详细接口描述
### 病人列表接口
#### 病人列表请求
|    字段     |  类型  |     备注     |
| ----------- | ------ | ------------ |
| size        | string | 单页行数     |
| page        | string | 请求页数     |
| patientId   | string | 请求病人ID   |
| patientName | string | 请求病人姓名 |
| gender      | string | 请求性别     |
| modality    | string | 请求检查手段 |
| studyDate   | date | 请求检查时间 |

#### 病人列表
|       字段       |  类型  |   备注   |
| ---------------- | ------ | -------- |
| content[]        | number | 病人数据 |
| totalPages       | number | 总页数   |
| totalElements    | number | 总记录数 |
| size             | number | 单页行数 |
| number           | number | 当前页数 |
| numberOfElements | number | 该页行数 |

#### 病人数据
|       字段       |  类型  |   备注   |
| ---------------- | ------ | -------- |
| patientId        | string | 病人ID   |
| patientName      | string | 病人姓名 |
| gender           | string | 性别     |
| modality         | string | 检查手段 |
| studyDescription | string | 检查概述 |
| studyDate        | string | 检查时间 |
