function [] = render_model(file_name)
    ITERATIONS = 3000;
    POINTS = 4;
    points = zeros(POINTS, 3, ITERATIONS);
    limit = 1;
    
    fid = fopen(file_name);

    i = 1;
    j = 1;
    line = fgetl(fid);
    while ischar(line)
        if strcmp(line,'')
            j = j + 1;
            i = 1;
            line = fgetl(fid);
            continue;
        end
        r = textscan(line, '%f');
        points(i,:,j) = r{1};
        line = fgetl(fid);
        i = i + 1;
    end

    fclose(fid);
    
    for i =1:ITERATIONS
        X = points(:,1,i);
        Y = points(:,2,i);
        Z = points(:,3,i);
        DT = delaunay(X,Y,Z);
        %array of tetrahedron handles
        h = tetramesh(DT,[X,Y,Z]);
        xlim([-limit limit])
        ylim([-limit limit])
        zlim([-limit limit])
        pause(0.1);
    end
    %ydata = get(h,'YData')
    %ydata
end