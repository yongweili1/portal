window.appConfig = {
    messagingUrl: "http://localhost:9090",
    //localeUrl: 'http://localhost:9000/api/i18n/',
    apiUrl: 'http://10.9.19.148:8000',
    locale: 'en_US',
    theme: 'lightness',
    name: 'uAI-Research-base',
    title: '系统管理',
    company: {
        address: ' ',
        website: null,
        copyright: '@2018 联影智能',
        qrcode: null
    }
};

var showcps = true;

// unit: ms
var delta_time = 100;

var delta_count = 4;

var shapes = {
    line: 'line',
    circle: 'circle',
    rectangle: 'rectangle',
    freepen: 'freepen',
    freepen_edit: 'freepen_edit'
}

var actions = {
    locate: 'locate',
    zoom: 'zoom',
    pan: 'pan',
    rotate: 'rotate',
    window: 'window',
    
    reset: 'reset',
    
    shape: 'shape',
    clear: 'clear',
    select: 'select'
}