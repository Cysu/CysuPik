%% Configuration Parameters
clc;
clear;
close all;

% image file
image = 'haze_6.jpg';
scale = 0.5;

% used when generating the prior: width of the patch that is examined
% to determine the dark channel value for a point
patch_size = 15;

% denoted by lowercase omega in He, Sun, Tang paper
% fraction of haze to remove
% leaving some (removal_amount < 1.0) preserves 'aerial perspective'
removal_amount = 0.95;

%% Some initial setup

im = imresize(imread(image), scale);
im = double(im)./255;
dims = size(im);
pixels = dims(1)*dims(2);

%% Compute the dark channel (non-normalized)

dark = demo_darkchannel(im, patch_size);

%% Calculate atmospheric light using dark channel

% fraction of pixels to consider for the atmospheric light calculation
count = ceil(.001*pixels);

[~, ix] = sort(reshape(dark, 1, []), 'descend');
ix = ix(1:count);
flatimage = reshape(im, [], 3);

intensity_sum = zeros(1,3);
for index = ix
    intensity_sum = intensity_sum + flatimage(index, :);
end

A = intensity_sum/count;

%% Calculate estimated transmission map

A_matrix = zeros(dims(1), dims(2), 3);
A_matrix(:,:,1) = A(1);
A_matrix(:,:,2) = A(2);
A_matrix(:,:,3) = A(3);

% eqn 12 from HST paper
trans_est = 1 - removal_amount * demo_darkchannel(im./A_matrix, patch_size);

%% Matting the transmission map
% Code in this section was partly inspired by code originally from
% http://www.soe.ucsc.edu/classes/ee264/Winter10/Proj6-Code.zip

win_size = 3; % window size
win_els = win_size.^2; % number of window elements
l_els = win_els.^2; % number of elements calculated in each iteration

win_bord = floor(win_size./2); 

e = 0.000001; 

[m,n,c] = size(im);
numpix = m*n;

k = reshape(1:numpix, m, n);
U = eye(win_size);
D = eye(win_els);

num_els = l_els*(m-2*win_bord)*(n-2*win_bord); 

ind_i = ones(1,num_els);
ind_j = ind_i;

els = zeros(1,num_els);

count = 0;

for x = (1 + win_bord):(n - win_bord)
    for y = (1 + win_bord):(m - win_bord)
        
        wk = reshape(im(y-win_bord:y+win_bord,x-win_bord:x+win_bord,:), win_els, c);
            
        w_ind = reshape(k(y-win_bord:y+win_bord,x-win_bord:x+win_bord), 1, win_els);
            
        [i j] = meshgrid(w_ind, w_ind);
        
        i = reshape(i,1,l_els);
        j = reshape(j,1,l_els);
        
        ind_i((count*l_els + 1):(count*l_els+l_els)) = i;
        ind_j((count*l_els + 1):(count*l_els+l_els)) = j;

        win_mu = mean(wk)';

        win_cov = wk'*wk/win_els-win_mu*win_mu';

        dif = wk' - repmat(win_mu,1,win_els);
        
        elements = D - (1 + dif(:,:)'*inv(win_cov + e./win_els.*U)*dif(:,:))./win_els;

        els((count*l_els + 1):(count*l_els+l_els)) = reshape(elements,1,l_els);
        
        count = count + 1;
    end
end

L = sparse(ind_i, ind_j, els, numpix, numpix);

%% generate refined transmission map

% recommended value from HST paper
lambda = .0001;

% equation 15 from HST
t = (L + lambda .* speye(size(L))) \ trans_est(:) .* lambda;

t = t - min(t(:));
t = t ./ (max(t(:)));
t = t .* (max(trans_est(:)) - min(trans_est(:))) + min(trans_est(:));
t = reshape(t, size(trans_est));

%% generate and show the image

% t = trans_est;
t_sub = 0.1; % transmission map lower bound

dehazed = zeros(size(im));

% equation 16 from HST
for c = 1:3
    dehazed(:,:,c) = (im(:,:,c) - A_matrix(:,:,c))./(max(t, t_sub)) + A_matrix(:,:,c);
end

% perform histogram adjustment before showing the image
adjusted = applycform(dehazed, makecform('srgb2lab'));
adjusted(:,:,1) = adapthisteq(adjusted(:,:,1)/100)*100;
adjusted = applycform(adjusted, makecform('lab2srgb'));

% show the image after applying enhancement
imshow(dehazed);