function ret = darkchannel(im)

patch_size = 15;
[h, w, ~] = size(im);

% padding around the boundaries
padim = padarray(im, [floor(patch_size/2) floor(patch_size/2)], 'symmetric');

ret = zeros(h, w);
for y = 1:h
    for x = 1:w
        local_patch = padim(y:y+patch_size-1, x:x+patch_size-1, :);
        ret(y, x) = min(local_patch(:));
    end
end