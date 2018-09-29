import { Injectable } from '@angular/core';
declare var THREE: any;
declare var MarchingSquaresJS: any;
declare var d3: any;
declare var mat4: any;
declare var vec3: any;

@Injectable()

export class actionService {

    threeStart(myCanvas, lightPoint) {
        var geometry;
        var renderer;
        var width;
        var height;
        initThree();
        initCamera();
        initScene();
        initLight();
        initObject_JS();
        function initThree() {
            width = myCanvas.clientWidth;//获取画布「canvas3d」的宽
            height = myCanvas.clientHeight;
            renderer = new THREE.WebGLRenderer({ alpha: true });//生成渲染器对象（属性：抗锯齿效果为设置有效）将alpha参数传递给WebGLRenderer构造函数
            renderer.setSize(width, height);//指定渲染器的高宽（和画布框大小一致）
            myCanvas.appendChild(renderer.domElement);//追加 【canvas】 元素到 【canvas3d】 元素中
            renderer.setClearColor(0x000000, 0);//设置canvas背景色(clearColor)
        }
        var camera;//设置相机
        function initCamera() {
            camera = new THREE.OrthographicCamera(-width / 2, width / 2, height / 2, -height / 2, 1, 1000);
            camera.position.x = lightPoint[0];//设置相机的位置坐标
            camera.position.y = lightPoint[1];
            camera.position.z = lightPoint[2];
            camera.up.x = 0;//相机朝向
            camera.up.y = 1;
            camera.up.z = 0;
            camera.lookAt({ x: 0, y: 0, z: 0 });//设置视野的中心坐标
        }
        var scene;//设置场景
        function initScene() {
            scene = new THREE.Scene();
        }
        var light;//设置光源
        function initLight() {
            light = new THREE.DirectionalLight(0xFFFFFF, 1.0, 0);//设置平行光源
            light.position.set(lightPoint[0], lightPoint[1], lightPoint[2]);//设置光源向量
            scene.add(light);// 追加光源到场景
        }
        function initObject_JS() {    //设置物体
            var jsonloader = new THREE.JSONLoader();
            jsonloader.load('../../../assets/data/mascot.js',
                function(geometry, materials) {
                    var object = new THREE.Mesh(geometry, materials);
                    object.position.x = 0;
                    object.position.y = 10;
                    object.position.z = 0;
                    object.scale.set(0.25, 0.25, 0.25);
                    scene.add(object);
                    renderer.clear();
                    renderer.render(scene, camera);
                });
        }
    }

    dosefield(unts) {
        var arr = new Array();
        for (var i = 0; i < 79; i++) {
            arr[i] = new Array(i);
            for (var k = 0; k < 155; k++) {
                arr[i][k] = unts[155 * i + k];
            }
        }
        var intervals = [600, 1200, 1800, 2400, 3000, 3600, 4200, 4800, 5400, 6000, 6360, 20000];
        var data = arr;
        var xs = d3.range(0, data[0].length);
        var ys = d3.range(0, data.length);
        var isoBands = [];

        for (var i = 1; i < intervals.length; i++) {
            var lowerBand = intervals[i - 1];
            var upperBand = intervals[i];
            var band = MarchingSquaresJS.isoBands(
                data,
                lowerBand,
                upperBand - lowerBand, {
                    successCallback: function(band) {
                        // console.log('Band' + i + ':', band)
                    },
                    verbose: true
                }
            );
            isoBands.push({ "coords": band, "level": i, "val": intervals[i] });
        }
        drawLines('#isobands', isoBands, intervals);
        console.log(JSON.stringify(isoBands));

        function drawLines(divId, lines, intervals) {
            var marginBottomLabel = 0;
            var width = 890;
            var height = 688;
            var xScale = d3.scale.linear()
                .range([0, width])
                .domain([Math.min.apply(null, xs), Math.max.apply(null, xs)]);
            var yScale = d3.scale.linear()
                .range([0, height])
                .domain([Math.min.apply(null, ys), Math.max.apply(null, ys)]);
            var colours = d3.scale.linear().domain([intervals[0], intervals[1], intervals[2], intervals[3], intervals[4], intervals[5], intervals[6], intervals[7], intervals[8], intervals[9], intervals[10], intervals[11]])
                .range([
                    d3.rgb(0, 32, 96),
                    d3.rgb(0, 32, 96),
                    d3.rgb(102, 102, 1),
                    d3.rgb(0, 112, 192),
                    d3.rgb(0, 176, 240),
                    d3.rgb(0, 176, 80),
                    d3.rgb(146, 208, 80),
                    d3.rgb(255, 255, 0),
                    d3.rgb(255, 192, 0),
                    d3.rgb(255, 127, 80),
                    d3.rgb(255, 0, 0),
                    d3.rgb(255, 0, 255)
                ]);
            var svg = d3.select(divId).append("svg").attr("width", width).attr("height", height + marginBottomLabel);
            svg.selectAll("path").data(lines).enter().append("path").style("fill", function(d) {
                return colours(d.val);
            })
                // .style("stroke", "black")
                .style('opacity', 0.5)
                .attr("d", function(d) {
                    var p = "";
                    d.coords.forEach(function(aa) {
                        p += (d3.svg.line()
                            .x(function(dat) {
                                return xScale(dat[0]);
                            })
                            .y(function(dat) {
                                return yScale(dat[1]);
                            })
                            .interpolate("linear")
                        )(aa) + "Z";
                    });
                    return p;
                })
                .on('mouseover', function() {
                    d3.select(this).style('fill', d3.rgb(204, 185, 116));
                })
                .on('mouseout', function() {
                    d3.select(this).style('fill', function(d1) {
                        return colours(d1.val);
                    })
                });
        }
    }

}
