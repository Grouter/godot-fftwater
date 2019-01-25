shader_type spatial;

uniform sampler2D height_map;
uniform vec3 off = vec3(-1.0, 0.0, 1.0);
uniform vec2 size = vec2(1.0, 0.0);

vec4 get_data(in vec2 pos) {
	vec2 texsize = vec2(textureSize(height_map, 0));
	float h = texture(height_map, pos).r; // 11
	float wh = texture(height_map, pos + off.xy / texsize).r; // 01 l
	float eh = texture(height_map, pos + off.zy / texsize).r; // 21 r
	float sh = texture(height_map, pos + off.yx / texsize).r; // 10 u
	float nh = texture(height_map, pos + off.yz / texsize).r; // 12 d
	
	vec3 va = normalize(vec3(0.0, 1.0, eh - wh));
	vec3 vb = normalize(vec3(nh - sh, 1.0, 0.0));
	
	/*vec3 va = normalize(vec3(eh - wh, 1.0, 0.0));
	vec3 vb = normalize(vec3(0.0, 1.0, nh - sh));*/

	return vec4(cross(va, vb), h);
}

void vertex() {
	vec4 data = get_data(UV);
	NORMAL = data.xyz;
	VERTEX.y = data.w;
}

void fragment() {
	//ALBEDO = texture(height_map, UV).rgb;
}