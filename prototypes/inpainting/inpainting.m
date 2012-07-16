%% Do cleaning.

clc;
clear;
close all;

%% Setting parameters.

sourcefile = 'source6.png';
markupfile = 'markup6.png';
targetfile = 'target6.png';
demofile = avifile('demo6.avi', 'fps', 30);

patchsize_1 = 9;
patchsize_2 = 9;
d_threshold = 20;

%% Initialization work.

sourceimg = imread(sourcefile);
markupimg = imread(markupfile);
grayimg = rgb2gray(sourceimg);
targetimg = markupimg;
Labimg = RGB2Lab(targetimg);

[h, w, ~] = size(sourceimg);

markupregion = (markupimg(:,:,1)==0 & markupimg(:,:,2)==0 & markupimg(:,:,3)==255);
sourceregion = ~markupregion;

% init confidence term
c = double(sourceregion);
d = -0.1.*ones(h, w);

% init isophote, which is perpendicular to the gradient direction
[gx, gy] = gradient(double(grayimg));
ix = gy;
iy = -gx;
% [gx(:,:,1), gy(:,:,1)] = gradient(double(sourceimg(:,:,1)));
% [gx(:,:,2), gy(:,:,2)] = gradient(double(sourceimg(:,:,2)));
% [gx(:,:,3), gy(:,:,3)] = gradient(double(sourceimg(:,:,3)));
% gx = sum(gx, 3)/(3*255);
% gy = sum(gy, 3)/(3*255);
% ix = -gy;
% iy = gx;

% make demo
demo(1).cdata = sourceimg;
demo(1).colormap = [];
demofile = addframe(demofile, demo(1));
demo(2).cdata = markupimg;
demo(2).colormap = [];
demofile = addframe(demofile, demo(2));

%% Eliminating.

iter = 0;

while any(markupregion(:))
    % find the border, using the mask of erosion
    mask = 1.*ones(3); mask(2,2) = -8;
    border = filter2(mask, double(markupregion), 'same');
    borderpixs = find(border > 0);
    
    % calculate the normal to the border
    [nx, ny] = gradient(double(~markupregion));
    n = [nx(borderpixs(:)) ny(borderpixs(:))];
    n = normr(n);
    n(~isfinite(n)) = 0;
    
    % calculate the linear structure factor
    d(borderpixs) = abs(ix(borderpixs).*n(:, 1) + iy(borderpixs).*n(:, 2))+1e-6;
    
    % calculate the confidence of the border
    for p = borderpixs'
        if d(p) <= d_threshold
            patchsize = patchsize_1;
        else
            patchsize = patchsize_2;
        end
        [~, ~, localpatch] = getpatch(w, h, p, patchsize);
        c(p) = sum(c(localpatch)) / numel(localpatch);
    end
    
    % calculate the priority
    priority = c(borderpixs).*d(borderpixs);
    
    % find the border pixel with max priority
    [maxp, idx] = max(priority);
    p = borderpixs(idx);
    if d(p) < d_threshold
        patchsize = patchsize_1;
    else
        patchsize = patchsize_2;
    end
    [targetrows, targetcols, targetpatch] = getpatch(w, h, p, patchsize);
    
    r1 = targetrows(1); r2 = targetrows(2);
    c1 = targetcols(1); c2 = targetcols(2);
    
    % find the best fit source patch
    [sourcepixs, targetpixs, besti, bestj] = getfitpatch_fast(targetimg, sourceregion, markupregion, targetrows, targetcols);
        
    % fill the target patch with the best fit source patch
    targetimg(targetpixs) = sourceimg(sourcepixs);
    Labimg(targetpixs) = Labimg(sourcepixs);
    
    % update the confidence and isophote
    c(targetpixs(:,1)) = c(p);
    ix(targetpixs(:,1)) = ix(sourcepixs(:,1));
    iy(targetpixs(:,1)) = iy(sourcepixs(:,1));
    
    % update the regions    
    markupregion(r1:r2, c1:c2) = false;

    iter = iter + 1;
    fprintf('iter %d: remains %d, maxp = %f, d = %f, (%d,%d)-(%d,%d) => (%d,%d), %f\n', ...
        iter, sum(sum(markupregion)), maxp, d(p), ...
        r1, c1, r2, c2, besti, bestj, sqrt((r1-besti).^2 + (c1-bestj).^2));
    
    % generate a frame of demo
    demo(iter+2).cdata = targetimg;
    demo(iter+2).colormap = [];
    demofile = addframe(demofile, demo(iter+2));
end

imwrite(targetimg, targetfile, 'png');
demofile = close(demofile);

