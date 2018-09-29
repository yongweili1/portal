// import { TreeviewItem, TreeviewSelection, TreeviewI18nDefault } from '../treeview';

import { TreeviewI18nDefault } from '../treeview-i18n';
import { TreeviewItem, TreeviewSelection } from '../treeview-item';

export class DropdownTreeviewSelectI18n extends TreeviewI18nDefault {
    private internalSelectedItem: TreeviewItem;

    set selectedItem(value: TreeviewItem) {
        if (value && value.children === undefined) {
            this.internalSelectedItem = value;
        }
    }

    get selectedItem(): TreeviewItem {
        return this.internalSelectedItem;
    }

    getText(selection: TreeviewSelection): string {
        return this.internalSelectedItem ? this.internalSelectedItem.text : 'All';
    }
}
