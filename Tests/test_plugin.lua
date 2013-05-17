function getPlacements(width, height, frameCount)
	local maxSize = 2048
	local maxFramesPerRow = math.floor(maxSize/width)
	local framesPerRow = frameCount

	if maxFramesPerRow < frameCount then
		repeat
			framesPerRow = math.floor(framesPerRow / 2) + 1
		until framesPerRow * width < maxSize
	end

	local rows = math.floor(frameCount/framesPerRow) + 1

	local outWidth = framesPerRow * width
	local outHeight = rows * height

	local positions = {}
	
	for i = 1, frameCount do
		local x = ((i-1) * width) % outWidth;
		local y = math.floor(((i-1) * width) / outWidth) * height;
		positions[i] = Point(x,y)
	end
	
	return ReturnWrapper(positions, outWidth, outHeight)
end