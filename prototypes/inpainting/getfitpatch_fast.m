function [sourcepixs, targetpixs, besti, bestj] = getfitpatch_fast(img, sourceregion, markupregion, targetrows, targetcols)
%GETFITPATCH_FAST returns the best fit patch to the targetpatch

[H, W, ~] = size(img);
h = targetrows(2) - targetrows(1);
w = targetcols(2) - targetcols(1);

[ac, ar] = meshgrid(1:W, 1:H);
alpha = (ac-targetcols(1)).^2 + (ar-targetrows(1)).^2;
alpha = 1-exp(-alpha/400);
% alpha = ones(H, W);

best = c_getfitpatch(H, W, double(img), sourceregion, markupregion, ...
    targetrows(1), targetrows(2), targetcols(1), targetcols(2), ...
    alpha);

besti = floor(best(1));
bestj = floor(best(2));

% calculate the target pixels' index to be filled
needfilled = markupregion(targetrows(1):targetrows(2), targetcols(1):targetcols(2));
len = sum(sum(needfilled));
coffset = H*W;

count = 1;
targetpixs = zeros(len, 3);
for i = targetrows(1):targetrows(2)
    for j = targetcols(1):targetcols(2)
        if markupregion(i, j) == 1
            targetpixs(count, 1) = (j-1)*H+i;
            targetpixs(count, 2) = targetpixs(count, 1) + coffset;
            targetpixs(count, 3) = targetpixs(count, 2) + coffset;
            count = count + 1;
        end
    end
end

% calculate the source pixels' index to fill
count = 1;
sourcepixs = zeros(len, 3);
for i = 0:h
    for j = 0:w
        if markupregion(i+targetrows(1), j+targetcols(1)) == 1
            sourcepixs(count, 1) = (j+bestj-1)*H+i+besti;
            sourcepixs(count, 2) = sourcepixs(count, 1) + coffset;
            sourcepixs(count, 3) = sourcepixs(count, 2) + coffset;
            count = count + 1;
        end
    end
end