import { Observable } from 'rxjs';
import { Injectable } from '@angular/core';
import { URLSearchParams } from '@angular/http';
import { AppConfigService } from '../app.config';
import { AuthHttp } from '../core/auth-http';
import { Page, PageRequest } from '../shared/models/pagination';
import { TreeviewItem } from '../shared/treeview/index';


@Injectable()
export class DropdownTreeviewService {
  public items: TreeviewItem[] = [];

  constructor(
    private http: AuthHttp,
    private appConfig: AppConfigService) { }

  public getOperatorTree() {
    return this.http.get(`${this.appConfig.apiUrl}/api/sales-station-operator/nodes`)
      .map((res) => {
        return this.getTreeToTreeviewItem(res.json());
      });
  }
  public getDeptNode() {
    return this.http
      .get(`${this.appConfig.apiUrl}/api/depts/getDepartNode`)
      .map((res) => this.getTransNode(res.json()));
  }

  private getTreeToTreeviewItem(data) {
    this.items = [];
    if (data.length > 0) {
      data.forEach((res) => {
        const item = {
          text: res.deptName,
          value: '',
          Expand: true,
          // collapsed: true,
          // checked: false,
          children: []
        };
        if (res.operators.length > 0) {
          res.operators.forEach((result) => {
            const childrenItem = {
              text: result.realName,
              value: result.userId,
              // checked: false
            };
            item.children.push(childrenItem);
          });
        }
        this.items.push(new TreeviewItem(item));
      });
    }
    return this.items;
  }

  public getTransNode(data) {
    this.items = [];
    if (data) {
      const item = {
        text: data.data.deptName,
        value: data.data.deptId,
        Expand: true,
        children: []
      };
      if (data.children.length > 0) {
        item.children = this.getChildTree(data.children);
      }
      this.items.push(new TreeviewItem(item));
    }
    return this.items;
  }

  private getChildTree(data) {
    const items = [];
    data.forEach((res) => {
    const childrenItem = {
        text: res.data.deptName,
        value: res.data.deptId,
        Expand: true,
        children: []
      };
      if (res.children.length > 0) {
        childrenItem.children = this.getChildTree(res.children);
      }
      items.push(childrenItem);
    });
    return items;
  }

}
