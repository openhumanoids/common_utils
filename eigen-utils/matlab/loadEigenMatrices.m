function loaded = loadEigenMatrices(fname)
loaded = {};
fid = fopen(fname,'r');
while true
    rows = fread(fid, 1,'int32');
    if feof(fid)
        break;
    end
    
    cols = fread(fid, 1,'int32');
    typelen = fread(fid, 1,'int32');
    type = fread(fid, typelen,'int8=>char');
    m = fread(fid,[rows,cols],type);
    loaded = [loaded,m];
end
fclose(fid);

if length(loaded)==1
    loaded = loaded{1};
end


end