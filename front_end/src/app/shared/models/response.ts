export interface BindingError {
    objectName?: string; // 对象名
    fieldName?: string; // 字段名
    fieldValue?: string; // 字段值
    message?: string; // 提示信息
}

export interface ErrorResponse {
    code?: string; // 错误码
    error?: string; // 错误信息
    message?: string; // 提示信息
    exception?: string; // 异常信息
    path?: string; // URI path
    bindingErrors?: BindingError[]; // 表单校验信息
    timestamp?: number; // 时间戳
}

export interface ResultResponse<T> {
    code?: string; // 代码，自行定义
    message?: string; // 提示信息
    data?: T; // 数据
}

export interface SelectableData<T> {
    selected: boolean;
    data: T;
}

export interface TreeData<T> {
    data: T;
    leaf?: boolean;
    expanded?: boolean;
    type?: string;
    children: Array<TreeData<T>>;
}