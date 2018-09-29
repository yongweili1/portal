/**
 * totalPages: the total number of pages which was derived from (totalElements / size)
 * totalElements: the total number of rows/records.
 * size: the number of records per page, this was passed from the client via param size
 * number: the page number sent by the client
 */
export interface Page<T> {
  content: T[];
  totalPages: number;
  totalElements: number;
  size: number;
  number: number;
  numberOfElements: number;
}

export interface PageRequest {
  page: number;
  size: number;
  text?: string;
}
