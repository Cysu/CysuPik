%% Cleaning

clc;
clear;
close all;

%% Initialize

imfile = 'haze_3.jpg';
scale = 1;
im = imresize(imread(imfile), scale);
im = double(im)./255;

[h w ~] = size(im);

%% Calculate the dark channel

darkim = darkchannel(im);

%% Calculate atmospheric light

[~, idx] = sort(darkim(:), 'descend');
A = zeros(1, 3);
for i = 1:floor(h*w*0.001)
    x = ceil(idx(i)/h);
    y = mod(idx(i), h);
    if y == 0
        y = h;
    end
    for c = 1:3
        A(c) = A(c) + im(y, x, c);
    end
end
A = A/floor(h*w*0.001);

%% Calculate the trans est

tmp_im = im;
for c = 1:3
    tmp_im(:, :, c) = tmp_im(:, :, c)./A(c);
end
t = 1 - 0.95*darkchannel(tmp_im);

%% Soft matting

eps = 1e-6;

idx = reshape(1:h*w, h, w);
U = eye(3, 3);
D = eye(9, 9);

lsize = 81*(h-2)*(w-2);

idx_i = ones(1, lsize);
idx_j = ones(1, lsize);
val = zeros(1, lsize);

count = 0;
for y = 2:h-1
    for x = 2:w-1
        wk = reshape(im(y-1:y+1, x-1:x+1, :), 9, 3);
        u = mean(wk)';
        cov = wk'*wk/9 - u*u';
        dif = wk' - repmat(u, 1, 9);
        ele = D - (1 + dif' / (cov+eps.*U./9) * dif)./9;
        
        l = count*81+1;
        r = l + 80;
        count = count + 1; 
        
        w_idx = reshape(idx(y-1:y+1, x-1:x+1), 1, []);
        [i, j] = meshgrid(w_idx, w_idx);
        i = reshape(i, 1, []);
        j = reshape(j, 1, []);
        idx_i(l:r) = i;
        idx_j(l:r) = j;
        val(l:r) = ele(:);            
    end
end

L = sparse(idx_i, idx_j, val, h*w, h*w);

tmp_t = t;
t = (L + 1e-4.*speye(size(L))) \ tmp_t(:) .* 1e-4;

% normalize
t = (t - min(t(:))) ./ max(t(:));
t = t .* (max(tmp_t(:))-min(tmp_t(:))) + min(tmp_t(:));
t = reshape(t, size(tmp_t));
tmp_t = t;

%% Recovery the scene radiance

t = max(tmp_t, 0.3);
J = zeros(size(im));
for c = 1:3
    J(:,:,c) = (im(:,:,c) - A(c))./t + A(c);
end
imshow(J);
