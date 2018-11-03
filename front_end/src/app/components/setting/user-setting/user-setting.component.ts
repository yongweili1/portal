import { Component, OnInit } from '@angular/core';
import { WebSocketService } from '../../../services/web-socket.service'

@Component({
    selector: 'mpt-user-setting',
    templateUrl: './user-setting.component.html',
    styleUrls: ['./user-setting.component.less']
})
export class UserSettingComponent implements OnInit {

    constructor(private webSocket: WebSocketService) { }

    ngOnInit() {
    }
    WebSocketTest() {
        this.webSocket.createObservableSocket().subscribe(result => {
            console.log(result)
        },(error)=>console.log(error),()=>console.log('WebSocket closed'))
    }
    SendMessage(){
        this.webSocket.sendMessage("hi, this is client");
    }
    closeWebSocket(){
        this.webSocket.closeWebSocket();
    }
}
