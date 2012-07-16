function [rows, cols, ret] = getpatch(w, h, p, patchsize)
%GETPATCH returns the index of pixels in the patch

c = floor((p-1)/h)+1;
r = rem(p-1, h)+1;
pr = floor(patchsize/2);

rows = [max(1, r-pr), min(h, r+pr)];
cols = [max(1, c-pr), min(w, c+pr)];

ret = zeros(1, (rows(2)-rows(1)+1)*(cols(2)-cols(1)+1));

count = 1;
for i = rows(1):rows(2)
    for j = cols(1):cols(2)
        ret(count) = (j-1)*h + i;
        count = count+1;
    end
end