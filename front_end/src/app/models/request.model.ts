export enum Order {
  ASC = 'asc',
  DESC = 'desc'
}

export interface RequestOrder {
  sort?: Order;
  sortField?: string;
}

export interface RequestPagination {
  page: number;
  size: number;
}

export interface RequestTime {
  beginTime?: string;
  endTime?: string;
}


export interface SortEvent {
  order: SortEventOrder;
  field: string;
}

export enum SortEventOrder {
  ASC = 1,
  DESC = -1
}

export interface PaginationEvent {
  size: number;
  page: number;
  order?: string;
  sortF?: string;
}

export interface Sort {
  ascending: boolean;
  descending: boolean;
  direction: string;
  ignoreCase: boolean;
  nullHandling: string;
  property: string;
}
export interface PageResponse<T> {
  content: T[];
  first?: boolean;
  last?: boolean;
  number?: number;
  numberOfElements?: number;
  size?: number;
  totalElements: number;
  totalPages?: number;
  sort?: Sort[];
}
