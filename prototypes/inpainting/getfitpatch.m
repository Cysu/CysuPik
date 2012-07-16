function [sourcepixs, targetpixs] = getfitpatch(img, sourceregion, markupregion, targetrows, targetcols)
%GETFITPATCH returns the best fit patch to the targetpatch

[H, W, ~] = size(img);
h = targetrows(2) - targetrows(1);
w = targetcols(2) - targetcols(1);

needfilled = markupregion(targetrows(1):targetrows(2), targetcols(1):targetcols(2));

targetpatch = double(img(targetrows(1):targetrows(2), targetcols(1):targetcols(2), :));
for c = 1:3
    targetpatch(:,:,c) = targetpatch(:,:,c).*(~needfilled);
end

mindif = 1e10;
besti = -1;
bestj = -1;

for j = 1:W-w
    for i = 1:H-h
        % check if the patch contains markupregion
        if numel(find(sourceregion(i:i+h, j:j+w)==0)) > 0
            continue;
        end
        
        sourcepatch = double(img(i:i+h, j:j+w, :));
        
        % calculate the difference
        dif = 0;
        for c = 1:3
            sourcepatch(:,:,c) = sourcepatch(:,:,c).*(~needfilled);
            dif = dif + sum(sum((targetpatch(:,:,c)-sourcepatch(:,:,c)).^2));
            if dif > mindif
                break;
            end
        end
            
        
        if dif < mindif
            mindif = dif;
            besti = i;
            bestj = j;
        end
    end
end

% calculate the target pixels' index to be filled
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