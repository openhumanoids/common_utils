function saveMatricesForEigen(fname, mats, saveAsFloat)
if (nargin<3)
    saveAsFloat = false;
end
fid = fopen(fname,'w');

if (saveAsFloat)
    savetype = 'single';
    type = 'float';
else
    savetype = 'double';
    type = 'double';
end
typelen =length(type);

if (~iscell(mats))
    mats = {mats};
end

for i=1:length(mats)
    [rows cols] = size(mats{i});
    written = fwrite(fid,rows,'int32');
    written = fwrite(fid,cols,'int32');
    written = fwrite(fid,typelen,'int32');
    written = fwrite(fid,type,'char');
    assert(written == typelen)
    written = fwrite(fid,mats{i},savetype);
end
fclose(fid);

end