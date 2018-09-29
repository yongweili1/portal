import { Component, ElementRef, OnInit, ViewChild } from '@angular/core';
import { FormBuilder, FormControl, FormGroup, Validators } from '@angular/forms';
import { ToastService } from '../../core/toast.service';
import { Router } from '@angular/router';

@Component({
  selector: 'mpt-setting',
  templateUrl: './setting.component.html',
  styleUrls: ['./setting.component.less']
})
export class SettingComponent implements OnInit {
  public form: FormGroup;
 


  @ViewChild('actorPicker') private actorPicker: ElementRef;
  @ViewChild('confirmerPicker') private confirmerPicker: ElementRef;
  @ViewChild('checkerPicker') private checkerPicker: ElementRef;

  constructor(private fb: FormBuilder,
    private router: Router,
    private toastService: ToastService,
    ) {
  }

  public showSelectActor() {
    (<any>this.actorPicker).show();
  }

  public showSelectConfirmer() {
    (<any>this.confirmerPicker).show();
  }

  public showSelectChecker() {
    (<any>this.checkerPicker).show();
  }

  ngOnInit() {
    this.buildForm();
  }


  public onSave() {
    if (!this.form.valid) {
      return false;
    }
    let startTimestamp = this.form.controls['plannedStartTime'].value.getTime();
    let endTimestamp = this.form.controls['plannedEndTime'].value.getTime();
    let clone_form = this.form.value;
    clone_form.plannedStartTime = startTimestamp;
    clone_form.plannedEndTime = endTimestamp;

  }


  private buildForm() {
    this.form = this.fb.group({
      title: new FormControl('', [Validators.required, Validators.minLength(1), Validators.maxLength(256)]),
      description: new FormControl('', [Validators.required, Validators.minLength(1), Validators.maxLength(512)]),
      bizType: new FormControl('1'),
      priority: new FormControl('', Validators.required),
      plannedStartTime: new FormControl('', Validators.required),
      plannedEndTime: new FormControl('', Validators.required),
      confirmer: new FormControl(''),
      actor: new FormControl(''),
      checker: new FormControl(''),
      confirmerName: new FormControl(''),
      actorName: new FormControl(''),
      checkerName: new FormControl('')
    });
  }

}
