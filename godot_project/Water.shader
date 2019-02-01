shader_type spatial;

uniform sampler2D height_map;
uniform sampler2D folding_map;

uniform float lambda = -5.0;
uniform float foam_treshold = -0.8;

uniform vec3 off = vec3(-1.0, 0.0, 1.0);
uniform vec2 size = vec2(2.0, 0.0);

vec3 get_normal(in vec2 pos) {
	vec2 texsize = vec2(textureSize(height_map, 0));
	float l = texture(height_map, pos + off.xy / texsize).g; // 01
	float r = texture(height_map, pos + off.zy / texsize).g; // 21
	float u = texture(height_map, pos + off.yx / texsize).g; // 10
	float d = texture(height_map, pos + off.yz / texsize).g; // 12
	
	vec3 va = normalize(vec3(size.xy, r - l));
	vec3 vb = normalize(vec3(size.yx, d - u));

	return normalize(cross(va, vb));
}

float schlick(in vec3 I, in vec3 N) {
	float theta = clamp(dot(normalize(I.xyz), normalize(N.xyz)), -1.0, 1.0);
	float R0 = 0.02037;
	return R0 + (1.0 - R0) * pow(1.0 - theta, 5);
}

float jacobian(in vec2 dis) {
	float jxx = 1.0 + lambda * length(dFdx(dis));
	float jyy = 1.0 + lambda * length(dFdy(dis));
	float jyx = lambda * length(dFdx(dis));
	
	return jxx * jyy - jyx * jyx;
}

void vertex() {
	vec3 data = texture(height_map, UV).rgb;
	NORMAL.xyz = get_normal(UV);
	VERTEX.y = data.y;
	VERTEX.xz += data.xz;
}

void fragment() {
	float foam = jacobian(texture(height_map, UV).xz);
	if(foam < foam_treshold) {
		ALBEDO.rgb = vec3(1.0 - foam);
	} else {
		ALBEDO.rgb = vec3(0.0, 0.4, 0.8);
	}
}
