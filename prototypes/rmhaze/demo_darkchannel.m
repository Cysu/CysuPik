function J = darkchannel(I, patch_size)
    [y x ~] = size(I);
    J = zeros(y, x);
    
     % add some space around the edges
     I = padarray(I, [floor(patch_size/2) floor(patch_size/2)], 'symmetric');
 	
     for m = 1:y
         for n = 1:x
             patch = I(m:(m+patch_size-1), n:(n+patch_size-1),:);
             J(m,n) = min(patch(:));
         end
     end
    
end


