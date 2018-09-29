import { Component, Injectable, Input, Output, EventEmitter, ViewChild, OnChanges, SimpleChanges } from '@angular/core';
import * as _ from 'lodash';
import { DropdownTreeviewSelectI18n } from './dropdown-treeview-select-i18n';
import { TreeviewI18nDefault, TreeviewI18n } from '../treeview-i18n';
import { TreeviewConfig } from '../treeview-config';
import { TreeviewItem } from '../treeview-item';
import { DropdownTreeviewComponent } from '../dropdown-treeview.component';
import { TreeviewComponent } from '../treeview.component';
import { DropdownDirective } from '../dropdown.directive';
import { TreeviewHelper } from '../treeview-helper';

@Component({
    selector: 'ngx-dropdown-treeview-select',
    templateUrl: './dropdown-treeview-select.component.html',
    styleUrls: [
        './dropdown-treeview-select.component.less'
    ],
    providers: [
        { provide: TreeviewI18n, useClass: DropdownTreeviewSelectI18n }
    ]
})
export class DropdownTreeviewSelectComponent implements OnChanges {
    @Input() config: TreeviewConfig;
    @Input() items: TreeviewItem[];
    @Input() value: any;
    @Output() valueChange = new EventEmitter<any>();
    @ViewChild(DropdownTreeviewComponent) dropdownTreeviewComponent: DropdownTreeviewComponent;
    filterText: string;
    private dropdownTreeviewSelectI18n: DropdownTreeviewSelectI18n;
    treeviewComponent: TreeviewComponent;
    dropdownDirective: DropdownDirective;

    constructor(
        public i18n: TreeviewI18n
    ) {
        this.config = TreeviewConfig.create({
            hasAllCheckBox: false,
            hasCollapseExpand: false,
            hasFilter: true,
            maxHeight: 200
        });
        this.dropdownTreeviewSelectI18n = i18n as DropdownTreeviewSelectI18n;
    }

    ngOnChanges(changes: SimpleChanges) {
        if (_.isNil(this.value)) {
            this.selectAll();
        } else {
            this.updateSelectedItem();
        }
    }

    select(item: TreeviewItem) {
        if (item.children === undefined) {
            this.selectItem(item);
        }
    }

    private updateSelectedItem() {
        if (!_.isNil(this.items)) {
            const selectedItem = TreeviewHelper.findItemInList(this.items, this.value);
            if (selectedItem) {
                this.selectItem(selectedItem);
            } else {
                this.selectAll();
            }
        }
    }

    private selectItem(item: TreeviewItem) {
        this.dropdownTreeviewComponent.dropdownDirective.close();
        if (this.dropdownTreeviewSelectI18n.selectedItem !== item) {
            this.dropdownTreeviewSelectI18n.selectedItem = item;
            if (this.value !== item.value) {
                this.value = item.value;
                this.valueChange.emit(item.value);
            }
        }
    }

    private selectAll() {
        const allItem = this.dropdownTreeviewComponent.treeviewComponent.allItem;
        this.selectItem(allItem);
    }
}
