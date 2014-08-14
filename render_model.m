function [] = render_model(file_name_points, file_name_forces)
    ITERATIONS = 300;
    POINTS = 242;
    STEP_TIME = 0.3;
    points = zeros(POINTS, 3, ITERATIONS);
    forces = zeros(POINTS, 3, ITERATIONS);
    force_mag = zeros(POINTS,ITERATIONS);
    average_force_mag = zeros(1,ITERATIONS);
    limit = 7;
    
    % load the position data
    points = load_data(file_name_points, points);
	% return if values are all still zero
	if ~all(all(points))
		return;
	end

    % load the force data
    forces = load_data(file_name_forces, forces);
	% return if values are all still zero
	if ~all(all(forces))
		return;
	end
    
    %force_vs_time()
    for i =1:ITERATIONS
        plot_points(i);
        %plot_forces();
        %plot_normals();
        pause(STEP_TIME);
    end
    
    % plots the points as a triangulated mesh
    function [] = plot_points(time_step)
        X = points(:,1,time_step);
        Y = points(:,2,time_step);
        Z = points(:,3,time_step);
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
                average_force_mag(i) = average_force_mag(i) +...
                    norm(forces(j,:,i));
            end
            average_force_mag(i) = average_force_mag(i)/POINTS;
        end
        x = 1:ITERATIONS;
%         %plot four smaple points
%         plot(x, force_mag(1,:),...
%              x, force_mag(27,:),...
%              x, force_mag(32,:),...
%              x, force_mag(54,:))
%        ylim([0 max(max(force_mag(:,:)))])
        plot(x, average_force_mag(1,:))
        ylim([0 max(max(average_force_mag(:,:)))])
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

% plots the forces as vectors at the points
    function [] = plot_normals()
        X = ones(POINTS, 1);
        Y = ones(POINTS, 1);
        Z = ones(POINTS, 1);
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
		try
            fid = fopen(file_name);
		catch ME
			fprintf('Error opening file %s\n', file_name);
			return
		end

        i = 1;
        j = 1;

		try
        	line = fgetl(fid);
		catch ME
			fprintf('Error reading file %s\n', file_name);
			return
		end

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
