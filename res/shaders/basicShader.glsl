 #version 130 

uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightPosition;
uniform ivec4 sizes; //{number of objects , number of lights , width, hight}  
uniform float dis;

in vec3 position1;
in vec2 texCoord0;

vec3 color(vec4 p, int index, vec3 v);




vec4 sphereIntersection(vec3 p0, vec3 v, vec4 sphere)
{
    vec3 O = sphere.xyz;
    //v = normalize(v);
    float radius = sphere.w;
    float radius2 = pow(radius, 2);
    vec3 L = O - p0;
    float tm = dot(L, v);
    float d = pow(length(L), 2) - pow(tm, 2);
    if(d > radius2)
        return vec4(0, 0, 0, 0);
    float th = sqrt(radius2 - d);
    float t = tm - th;
    if (t < 0)
        t = tm + th;
	if(t>0)
		return vec4((p0 + t*v), 1);
	else
		return vec4(0,0,0,0);
}

vec4 planeIntersection(vec3 p0, vec3 v, vec4 plane)
{
	v = normalize(v);
	vec3 N = normalize(plane.xyz);
	float m;
	vec3 Q0 = vec3(0,0,0);
	if(plane.z !=0)	{
		m = -plane.w/plane.z;
		 Q0 = vec3(0,0,m);
		}
	else if(plane.x !=0)
		{
		m = -plane.w/plane.x;
		 Q0 = vec3(m,0,0);
		}
	else{
		m = -plane.w/plane.y;
		 Q0 = vec3(0,m,0);
		}
	
	float t = /*(dot(N,(Q0-p0)))/(dot(v,N));*/(-plane.w-dot(N,p0))/(dot(N,v));
	if (t > 0)
		return vec4((p0+t*v),1);
	else
		return vec4(0,0,0,0);
}

vec3 reflection(vec4 p,vec3 v, int index)
{
	vec3 normal;
	for(int i = 0; i<5; i++)
	{
		vec4 p2 = vec4(0,0,0,0), p3 = vec4(0,0,0,0);
		bool mirror = false;
		float dist = 1000;
		int index2;
		if(objects[index].w > 0)
		normal = (p.xyz-objects[index].xyz)/length(p.xyz-objects[index].xyz);
		else
		{
			normal = -normalize(objects[index].xyz);
		}
		vec3 r = normalize(v - 2*dot(v,normal)*normal);
		for(int j =0; j<sizes.x; j++)
		{
			if(objects[j].w >0)
				{
					p2 = sphereIntersection(p.xyz, r, objects[j]);
				}
				else
				{
				
					p2 = planeIntersection(p.xyz, r, objects[j]);
				}
				if(p2 != vec4(0,0,0,0) && objects[j]!=objects[index]  && distance(p2.xyz , p.xyz) < dist)
					{
						if(j<sizes.z && i < 4)
						{
							dist = distance(p2.xyz , p.xyz);
							p3 = p2;
							index2 = j;
							mirror = true;
						}
						else
						{
							dist = distance(p2.xyz , p.xyz);
							p3 = p2;
							index2 = j;
							mirror = false;
						}
					}
		}
		if (mirror)
		{
			index = index2;
			p = p3;
			v = r;
		}
		else if(p3 != vec4(0,0,0,0))
				return color(p3, index2, r);

	}
	return vec3(0,0,0);

}

vec3 color(vec4 p, int index, vec3 v)
{
	vec3 ka = objColors[index].xyz;
	vec3 ia = ambient.xyz;
	vec3 normal;
	bool isPlane = false;
	if(objects[index].w > 0)
		normal = normalize(p.xyz-objects[index].xyz);
	else
	{
		normal = -normalize(objects[index].xyz);
		isPlane = true;
	}
	vec3 sum = vec3(0,0,0);
	vec3 ks =vec3(0.7,0.7,0.7);
	for(int i = 0; i< sizes[1]; i++)
	{
		vec3 Li;
		vec3 d;
		vec3 Kd = ka;
		float si = 1.0;
		if(lightsDirection[i].w == 1.0)
		{
			vec3 light = lightPosition[i].xyz;
			Li = normalize(light - p.xyz);
			d = normalize(p.xyz - lightPosition[i].xyz);
			if(dot(d,normalize(lightsDirection[i].xyz)) < lightPosition[i].w)
				continue;
		}
		else
		{
			Li =  -normalize(lightsDirection[i].xyz);
			d = normalize(lightsDirection[i].xyz);
		}
		vec3 Ri = normalize(d - 2*dot(d,normal)*normal);
		vec3 Ii = lightsIntensity[i].xyz;
		float a = 1.0;
		if(isPlane)
		{
			vec4 p1 = p;
			if(p1.x * p1.y <= 0)
			{
				if(p1.x < 0)
					p1.x = mod(int(p1.x * 1.5), 2) + 2;
				else
					p1.y = mod(int(p1.y * 1.5), 2) + 2;
			}
			if(mod(int(1.5*p1.x),2) == mod(int(1.5*p1.y),2))
			a = 0.5;	
		}
		for(int j =0; j<sizes[0]; j++)
			{
				if(objects[j].w >0)
				{
					vec4 p2 = sphereIntersection(p.xyz, -d, objects[j]);
					if(p2 != vec4(0,0,0,0) && objects[j]!=objects[index])
					{
						if((lightsDirection[i].w == 1.0 && length(lightPosition[i].xyz - p2.xyz) < length(lightPosition[i].xyz-p.xyz)))
						{
							si = 0.0;
							break;
						}
						else if( lightsDirection[i].w ==0)
							{
							si = 0.0;
							break;
							}
					}
				}
				else
				{
					vec4 p2 = planeIntersection(p.xyz, -d, objects[j]);
					if(p2 != vec4(0,0,0,0) && objects[j]!=objects[index])
						if((lightsDirection[i].w == 1.0 && length(lightPosition[i].xyz - p2.xyz)<length(lightPosition[i].xyz-p.xyz)) || lightsDirection[i].w ==0)
							si = 1.0;
				}
						
			}
		sum += (Kd*dot(normal,Li)*a + ks*pow(dot(v,Ri), objColors[index].w))*Ii*si;
	}
	return ka*ia + clamp(sum, 0.0, 1.0);

}

vec3 colorCalc( vec3 intersectionPoint)
{
	

	vec3 vTo = vec3(0,0,-1);
	vec3 vUp = vec3(0,-1,0);
	vec3 vRight = normalize(cross(vUp,vTo));
	vec3 vUp1 = normalize(cross(vRight,vTo));
	vec3 pc = eye.xyz + dis*vTo;
	vec3 p = pc + 2*(texCoord0.x-0.5)*vRight + 2*(texCoord0.y-0.5)*vUp1;
	vec3 v = normalize(p - eye.xyz);
	float dist = 1000.0;
	vec3 toReturn = vec3(0,0,0);
	for(int i = 0; i<sizes.x; i++){
		vec4 object = objects[i];
		if(object.w > 0) {
			vec4 p = sphereIntersection(eye.xyz, v, object);
			if(p != vec4(0,0,0,0) && distance(p.xyz , eye.xyz) < dist)
			{
				dist = distance(p.xyz , eye.xyz);
				if(i<sizes.z)
					toReturn = reflection(p,v,i);
				else
					toReturn = color(p,i,v); 
				}
		}
		else{
			vec4 p = planeIntersection(eye.xyz, v, object);
			if(p != vec4(0,0,0,0) && distance(p.xyz , eye.xyz) < dist)
			{
				dist = distance(p.xyz , eye.xyz);
				if(i<sizes.z)
					toReturn = reflection(p,v,i);
				else
					toReturn = color(p,i,v); 
				}
			}
	}

    return toReturn;
}


void main()
{  
   
            gl_FragColor = vec4(colorCalc(eye.xyz),1);      
}
 
