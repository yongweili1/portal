export class glsource {
    mainVertS = `
        attribute vec4 aVertexPosition;
        attribute vec2 aTextureCoord;
    
        uniform mat4 uModelViewMatrix;
        uniform mat4 uProjectionMatrix;

        varying highp vec2 vTextureCoord;
    
        void main() {
          gl_Position = uProjectionMatrix * uModelViewMatrix * aVertexPosition;
          vTextureCoord = aTextureCoord;
        }
      `;

    // Fragment shader program 片段着色器 将顶点转化为像素的技术称为“图元光栅化”
    mainFragS = `
        precision mediump float;
        varying vec2 vTextureCoord;
        uniform sampler2D uSampler;

        uniform vec2 rescale;
        uniform vec2 windowLevel;

        void main() {
          // slope = 1; intercept= -1000; wl= 60; ww= 400
          //float gray = texture2D(uSampler, vTextureCoord).r - 1000.0;
          vec4 t1= texture2D(uSampler, vTextureCoord);
          float pv1= ((4096.0 * t1.x) + (256.0 * t1.y) + (16.0 * t1.z) + t1.w) * 15.0;
          float gray = pv1*rescale.x + rescale.y;
          float min = windowLevel.x - windowLevel.y/2.0;
          float max = windowLevel.x + windowLevel.y/2.0;
          if(gray < min)
          {
            gray = 0.0;
            //gl_FragColor = vec4(0.0, 0.0, 0.5, 1);
          }
          else if(gray > max)
          {
            gray = 1.0;
            //gl_FragColor = vec4(0.8, 0.0, 0.0, 1);
          }
          else
          {
            gray = (gray - min) / windowLevel.y;
          }
          gl_FragColor = vec4(gray, gray, gray, 1);
        }
      `;

    private offScreenVertS: string = `
      attribute vec3 aVertexPosition;
    
      uniform mat4 uModelViewMatrix;
      uniform mat4 uProjectionMatrix;
    
      void main() {
        gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aVertexPosition,1.0);
      }
    `;

    // Fragment shader program

    private offScreenFragS: string = `
      void main() {
        gl_FragColor = vec4(1.0, 0, 0, 1);
      }
    `;
}
