function ext = getExtension(fileName)
lastDot = find(fileName=='.',1,'last');
ext = fileName(lastDot+1:end);
end