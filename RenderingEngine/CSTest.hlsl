struct BuffType
{
	int i;
	float f;
	float2 padding;
};


cbuffer INPUT_DATA: register(b0)
{
	BuffType inputData;
}

RWTexture1D<float2> outputData :register(t0);


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 output;
	output.x = inputData.i + 5;
	output.y = inputData.f + 5;
	outputData[DTid.x] = output;
}