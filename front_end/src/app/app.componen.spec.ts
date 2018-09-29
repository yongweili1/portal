import { NO_ERRORS_SCHEMA } from '@angular/core';
import {
    inject,
    async,
    TestBed,
    ComponentFixture
} from '@angular/core/testing';

import { AuthService } from './core/auth.service';
import { AppComponent } from './app.component';
import { AppModule } from './app.module';

describe(`App`, () => {
    let comp: AppComponent;
    let fixture: ComponentFixture<AppComponent>;

    beforeEach(async(() => {
        TestBed.configureTestingModule({
            imports: [AppModule],
            schemas: [NO_ERRORS_SCHEMA],
        }).compileComponents();
    }));

    beforeEach(async(inject([AuthService], (authService: AuthService) => {
        authService.login('user', 'password').subscribe((res) => {
            console.log('用户已经登录。');
        });
    })));

    beforeEach(() => {
        fixture = TestBed.createComponent(AppComponent);
        comp = fixture.componentInstance;

        fixture.detectChanges();
    });

    it(`App 被创建`, () => {
        expect(fixture).toBeDefined();
        expect(comp).toBeDefined();
    });

    it('App 初始化', () => {
        spyOn(console, 'log');
        expect(console.log).not.toHaveBeenCalled();
    });

});
