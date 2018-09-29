export interface TotalSaleModel {
  UGS?: number;
  transactions?: number;
  dailyUGS?: number;
}

export interface TodaySaleModel {
  UGS?: number;
  transactions?: number;
  dayOnDay?: number;
}

export interface ArrearsModel {
  UGS?: number;
  ratio?: number;
  UGSperCapita?: number;
}

export interface RefundModel {
  UGS?: number;
  transactions?: number;
}

export interface SalesNumber {
  date?: Date;
  dateValue?: number;
}

export interface SalesTable {
  saleName?: string;
  UGS?: number;
}

export interface OperaNumber {
  normal?: number;
  balanceAlarm?: number;
  arrearage?: number;
}

export interface OperaTable {
  saleName?: string;
  timeAgo?: number;
  sold?: number;
}
