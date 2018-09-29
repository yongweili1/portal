import { Component, OnInit, Input, ChangeDetectionStrategy } from '@angular/core';

@Component({
  selector: 'mpt-icon-button',
  templateUrl: './icon-button.component.html',
  styleUrls: ['./icon-button.component.less'],
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class IconButtonComponent implements OnInit {
  @Input() label;
  @Input() icon;

  constructor() { }

  ngOnInit() {
  }

}
