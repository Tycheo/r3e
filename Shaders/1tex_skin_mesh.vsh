uniform int bindBone;
uniform mat4 bmats[32];
attribute vec4 bones;
attribute vec4 weights;

void main(void){
	vec4 outVert;
	vec4 curVert = gl_Vertex;
	if(bindBone == 0){
		for(int i = 0; i < 4; i++){
			outVert += weights[i] * (vec4(bmats[int(bones[i])] * curVert));
		}
	}else{
		outVert = curVert;//vec4(bmats[bindBone] * curVert);
	}
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = vec4(gl_ModelViewProjectionMatrix * outVert);
}