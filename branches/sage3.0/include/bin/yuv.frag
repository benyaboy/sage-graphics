#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect yuvtex;

void main(void)
{
    float nx, ny, tu, tv, ty;
    float t, dx, dy;
    float y,u,v;
    float r,g,b;
    int m, n;
    vec4 vy, uy, yv, yu;

    nx=gl_TexCoord[0].s;
    ny=gl_TexCoord[0].t;

    dx = gl_FragCoord.s;
    dy = gl_FragCoord.t;

    if(mod(floor(nx),2.0)>0.5) {

	uy = texture2DRect(yuvtex,vec2(nx-1.0,ny));
	vy = texture2DRect(yuvtex,vec2(nx,ny));

	u  = uy.x;
	v  = vy.x;

	y  = vy.a;
    }
    else {
	uy = texture2DRect(yuvtex,vec2(nx,ny));
	vy = texture2DRect(yuvtex,vec2(nx+1.0,ny));

	u  = uy.x;
 	v  = vy.x;

	y  = uy.a;
    }

     y=1.1643*(y-0.0625);
     u=u-0.5;
     v=v-0.5;
    
     r=y+1.5958*v;
     g=y-0.39173*u-0.81290*v;
     b=y+2.017*u;
     gl_FragColor=vec4(r,g,b,1.0);
}
