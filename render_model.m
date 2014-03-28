function [] = render_model(file_name_points, file_name_forces)
    ITERATIONS = 300;
    POINTS = 4;
    STEP_TIME = 0.1;
    points = zeros(POINTS, 3, ITERATIONS);
    forces = zeros(POINTS, 3, ITERATIONS);
    force_mag = zeros(POINTS,ITERATIONS);
    limit = 3;
    
    % load the position data
    points = load_data(file_name_points, points);
    % load the force data
    forces = load_data(file_name_forces, forces);
    
    force_vs_time()
    for i =1:ITERATIONS
        %plot_points();
        %plot_forces();
        %pause(STEP_TIME);
    end
    
    % plots the points as a triangulated mesh
    function [] = plot_points()
        X = points(:,1,i);
        Y = points(:,2,i);
        Z = points(:,3,i);
        % the set up points must be triangulated to make
        % a mesh
        DT = delaunay(X,Y,Z);
        % h is an array of tetrahedron handles
        h = tetramesh(DT,[X,Y,Z]);
        xlim([-limit limit])
        ylim([-limit limit])
        zlim([-limit limit])
    end
    
    function [] = force_vs_time()
        for i = 1:ITERATIONS
            for j = 1:POINTS
                force_mag(j,i) = norm(forces(j,:,i));
            end
        end
        x = 1:ITERATIONS;
        plot(x, force_mag(1,:),...
             x, force_mag(2,:),...
             x, force_mag(3,:),...
             x, force_mag(4,:))
    end

    % plots the forces as vectors at the points
    function [] = plot_forces()
        X = points(:,1,i);
        Y = points(:,2,i);
        Z = points(:,3,i);
        U = forces(:,1,i);
        V = forces(:,2,i);
        W = forces(:,3,i);
        
        for j = 1:POINTS
           fprintf( 'point %d: %f\n', j, norm([U(j) V(j) W(j)]))
        end
        fprintf('\n\n\n')
        
        quiver3(X,Y,Z,U,V,W)
        xlim([-limit limit])
        ylim([-limit limit])
        zlim([-limit limit])
    end

    function [array] = load_data(file_name, array)
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
            array(i,:,j) = r{1};
            line = fgetl(fid);
            i = i + 1;
        end
        fclose(fid);
        
    end
end